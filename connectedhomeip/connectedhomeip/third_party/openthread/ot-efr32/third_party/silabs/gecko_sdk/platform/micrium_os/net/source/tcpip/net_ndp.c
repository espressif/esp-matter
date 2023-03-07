/***************************************************************************//**
 * @file
 * @brief Network Ndp Layer - (Neighbor Discovery Protocol)
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

#ifdef  NET_NDP_MODULE_EN

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <em_core.h>

#include  "../../include/net_util.h"

#include  "net_ndp_priv.h"
#include  "net_ipv6_priv.h"
#include  "net_mldp_priv.h"
#include  "net_dad_priv.h"
#include  "net_if_priv.h"
#include  "net_cache_priv.h"
#include  "net_priv.h"

#include  <common/include/lib_utils.h>
#include  <common/source/collections/slist_priv.h>
#include  <common/source/rtos/rtos_utils_priv.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  LOG_DFLT_CH                                    (NET)
#define  RTOS_MODULE_CUR                                 RTOS_CFG_MODULE_NET

#define  NET_NDP_CACHE_FLAG_ISROUTER                     DEF_BIT_00

#define  NET_NDP_PREFIX_LEN_MAX                          128u

#define  NET_NDP_TWO_HOURS_SEC_NBR                       7200u
#define  NET_NDP_LIFETIME_TIMEOUT_MS                     NET_NDP_TWO_HOURS_SEC_NBR * 1000

#define  NET_NDP_MS_NBR_PER_SEC                          1000u

#define  NET_NDP_CACHE_TX_Q_TH_MIN                       0u
#define  NET_NDP_CACHE_TX_Q_TH_MAX                       NET_BUF_NBR_MAX

/********************************************************************************************************
 *                                       INTERFACE SELECTION DEFINES
 *
 * Notes : (1) This classification is used to found the good interface to Tx for a given destination
 *               address.
 *******************************************************************************************************/

#define  NET_NDP_IF_DEST_ON_LINK_WITH_SRC_ADDR_CFGD          7
#define  NET_NDP_IF_DEST_ON_LINK                             6
#define  NET_NDP_IF_DFLT_ROUTER_ON_LINK_WITH_SRC_ADDR_CFGD   5
#define  NET_NDP_IF_DFLT_ROUTER_ON_LINK                      4
#define  NET_NDP_IF_ROUTER_ON_LINK_WITH_SRC_ADDR_CFGD        3
#define  NET_NDP_IF_ROUTER_ON_LINK                           2
#define  NET_NDP_IF_SRC_ADDR_CFGD                            1
#define  NET_NDP_IF_NO_MATCH                                 0

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                               NDP NEIGHBOR SOLICITATION TYPE DATA TYPE
 *******************************************************************************************************/

typedef enum net_ndp_neighbor_sol_type {
  NET_NDP_NEIGHBOR_SOL_TYPE_DAD = 1u,
  NET_NDP_NEIGHBOR_SOL_TYPE_RES = 2u,
  NET_NDP_NEIGHBOR_SOL_TYPE_NUD = 3u,
} NET_NDP_NEIGHBOR_SOL_TYPE;

/********************************************************************************************************
 *                                       NDP HEADERS OPTIONS DATA TYPE
 *******************************************************************************************************/

typedef  struct  net_ndp_opt_hw_addr_hdr {
  NET_NDP_OPT_HDR Opt;                                                  // NDP opt type and length.
  CPU_INT08U      Addr[NET_IF_HW_ADDR_LEN_MAX];                         // NDP hw addr.
} NET_NDP_OPT_HW_ADDR_HDR;

typedef  struct  net_ndp_opt_prefix_info_hdr {
  NET_NDP_OPT_HDR Opt;                                                  // NDP opt type and length.
  CPU_INT08U      PrefixLen;                                            // NDP opt prefix info len (in bits).
  CPU_INT08U      Flags;                                                // NDP opt prefix info flags.
  CPU_INT32U      ValidLifetime;                                        // NDP opt prefix info valid    lifetime.
  CPU_INT32U      PreferredLifetime;                                    // NDP opt prefix info prefered lifetime.
  CPU_INT32U      Reserved;
  NET_IPv6_ADDR   Prefix;                                               // NDP opt prefix info prefix addr.
} NET_NDP_OPT_PREFIX_INFO_HDR;

typedef  struct  net_ndp_opt_redirect_hdr {
  NET_NDP_OPT_HDR Opt;                                                  // NDP opt type and length.
  CPU_INT16U      Reserved1;
  CPU_INT32U      Reserved2;
  CPU_INT08U      Data;                                                 // NDP IP header & data.
} NET_NDP_OPT_REDIRECT_HDR;

typedef  struct  net_ndp_opt_mtu_hdr {
  NET_NDP_OPT_HDR Opt;                                                  // NDP opt type and length.
  CPU_INT16U      Reserved;
  CPU_INT32U      MTU;                                                  // NDP MTU.
} NET_NDP_OPT_MTU_HDR;

/********************************************************************************************************
 *                                       NDP LAYER DATA DATA TYPE
 *******************************************************************************************************/

typedef  struct  net_ndp_data {
  CPU_INT08U     SolicitMaxAttempsMulticastNbr;                 // NDP mcast   solicit. max attempts nbr.
  CPU_INT08U     SolicitMaxAttempsUnicastNbr;                   // NDP unicast solicit. max attempts nbr.
  CPU_INT32U     SolicitTimeout_ms;                             // NDP solicitations timeout (in ms).
  CPU_INT32U     ReachableTimeout_ms;
#ifdef NET_DAD_MODULE_EN
  CPU_INT08U     DAD_MaxAttemptsNbr;
#endif
  NET_BUF_QTY    CacheTxQ_MaxTh_nbr;                            // Max nbr tx bufs to enqueue on NDP cache.
  NET_NDP_ROUTER **DfltRouterTbl;
  SLIST_MEMBER   *RouterListPtr;
  SLIST_MEMBER   *PrefixListPtr;
  SLIST_MEMBER   *DestListPtr;
  MEM_DYN_POOL   RouterPool;
  MEM_DYN_POOL   PrefixPool;
  MEM_DYN_POOL   DestPool;
#if (NET_STAT_POOL_NDP_EN == DEF_ENABLED)
  NET_STAT_POOL  RouterPoolStat;
#endif

#if (NET_STAT_POOL_NDP_EN == DEF_ENABLED)
  NET_STAT_POOL PrefixPoolStat;
#endif

#if (NET_STAT_POOL_NDP_EN == DEF_ENABLED)
  NET_STAT_POOL DestPoolStat;
#endif
} NET_NDP_DATA;

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

CPU_INT32U NetNDP_NeighborCacheTimeout_ms;                          // NDP Neighbor cache timeout     (in ms).
CPU_INT32U NetNDP_DelayTimeout_ms;                                  // NDP Neighbor delay timeout     (in ms).
CPU_INT16U NetNDP_CacheAccessedTh_nbr;                              // Nbr successful srch's to promote NDP cache.

static NET_NDP_DATA           *NetNDP_DataPtr;
static NET_NDP_NEIGHBOR_CACHE NetNDP_NeighborCacheTbl[NET_NDP_CFG_CACHE_NBR];       // Neighbor Cache Table.

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

static void NetNDP_RxRouterAdvertisement(const NET_BUF                *p_buf,
                                         NET_BUF_HDR                  *p_buf_hdr,
                                         const NET_NDP_ROUTER_ADV_HDR *p_ndp_hdr,
                                         RTOS_ERR                     *p_err);

static void NetNDP_RxNeighborSolicitation(const NET_BUF                  *p_buf,
                                          NET_BUF_HDR                    *p_buf_hdr,
                                          const NET_NDP_NEIGHBOR_SOL_HDR *p_ndp_hdr,
                                          RTOS_ERR                       *p_err);

static void NetNDP_RxNeighborAdvertisement(const NET_BUF                  *p_buf,
                                           NET_BUF_HDR                    *p_buf_hdr,
                                           const NET_NDP_NEIGHBOR_ADV_HDR *p_ndp_hdr,
                                           RTOS_ERR                       *p_err);

static void NetNDP_RxRedirect(const NET_BUF              *p_buf,
                              NET_BUF_HDR                *p_buf_hdr,
                              const NET_NDP_REDIRECT_HDR *p_ndp_hdr,
                              RTOS_ERR                   *p_err);

static void NetNDP_RxPrefixUpdate(NET_IF_NBR    if_nbr,
                                  NET_IPv6_ADDR *p_addr_prefix,
                                  CPU_INT08U    prefix_len,
                                  CPU_BOOLEAN   on_link,
                                  CPU_BOOLEAN   addr_cfg_auto,
                                  CPU_INT32U    lifetime_valid,
                                  CPU_INT32U    lifetime_preferred,
                                  RTOS_ERR      *p_err);

static void NetNDP_RxPrefixHandler(NET_IF_NBR          if_nbr,
                                   const NET_IPv6_ADDR *p_addr_prefix,
                                   CPU_INT08U          prefix_len,
                                   CPU_INT32U          lifetime_valid,
                                   CPU_BOOLEAN         on_link,
                                   CPU_BOOLEAN         addr_cfg_auto,
                                   RTOS_ERR            *p_err);

static CPU_BOOLEAN NetNDP_RxPrefixAddrsUpdate(NET_IF_NBR    if_nbr,
                                              NET_IPv6_ADDR *p_addr_prefix,
                                              CPU_INT08U    prefix_len,
                                              CPU_INT32U    lifetime_valid,
                                              CPU_INT32U    lifetime_preferred);

static void NetNDP_TxNeighborSolicitation(NET_IF_NBR                if_nbr,
                                          NET_IPv6_ADDR             *p_addr_src,
                                          NET_IPv6_ADDR             *p_addr_dest,
                                          NET_NDP_NEIGHBOR_SOL_TYPE ndp_sol_type,
                                          RTOS_ERR                  *p_err);

static CPU_BOOLEAN NetNDP_IsPrefixCfgdOnAddr(NET_IPv6_ADDR *p_addr,
                                             NET_IPv6_ADDR *p_addr_prefix,
                                             CPU_INT08U    p_refix_len);

static NET_NDP_ROUTER *NetNDP_UpdateDefaultRouter(NET_IF_NBR if_nbr);

static void NetNDP_UpdateDestCache(NET_IF_NBR       if_nbr,
                                   const CPU_INT08U *p_addr,
                                   const CPU_INT08U *p_addr_new);

static void NetNDP_RemoveAddrDestCache(NET_IF_NBR       if_nbr,
                                       const CPU_INT08U *p_addr);

static void NetNDP_RemovePrefixDestCache(NET_IF_NBR       if_nbr,
                                         const CPU_INT08U *p_prefix,
                                         CPU_INT08U       prefix_len);

static void NetNDP_NeighborCacheAddPend(const NET_BUF    *p_buf,
                                        NET_BUF_HDR      *p_buf_hdr,
                                        const CPU_INT08U *p_addr_protocol,
                                        RTOS_ERR         *p_err);

static NET_CACHE_ADDR_NDP *NetNDP_NeighborCacheAddEntry(NET_IF_NBR       if_nbr,
                                                        const CPU_INT08U *p_addr_hw,
                                                        const CPU_INT08U *p_addr_ipv6,
                                                        const CPU_INT08U *p_addr_ipv6_sender,
                                                        CPU_INT32U       timeout_ms,
                                                        CPU_FNCT_PTR     timeout_fnct,
                                                        CPU_INT08U       cache_state,
                                                        CPU_BOOLEAN      is_router,
                                                        RTOS_ERR         *p_err);

static void NetNDP_NeighborCacheUpdateEntry(NET_CACHE_ADDR_NDP *p_cache_addr_ndp,
                                            CPU_INT08U         *p_ndp_opt_hw_addr);

static void NetNDP_NeighborCacheRemoveEntry(NET_NDP_NEIGHBOR_CACHE *p_cache,
                                            CPU_BOOLEAN            tmr_free);

static CPU_BOOLEAN NetNDP_RouterDfltGet(NET_IF_NBR     if_nbr,
                                        NET_NDP_ROUTER **p_router,
                                        RTOS_ERR       *p_err);

static NET_NDP_ROUTER *NetNDP_RouterCfg(NET_IF_NBR    if_nbr,
                                        NET_IPv6_ADDR *p_addr,
                                        CPU_BOOLEAN   timer_en,
                                        CPU_FNCT_PTR  timeout_fnct,
                                        CPU_INT32U    timeout_ms,
                                        RTOS_ERR      *p_err);

static NET_NDP_ROUTER *NetNDP_RouterGet(RTOS_ERR *p_err);

static NET_NDP_ROUTER *NetNDP_RouterSrch(NET_IF_NBR    if_nbr,
                                         NET_IPv6_ADDR *p_addr,
                                         RTOS_ERR      *p_err);

static void NetNDP_RouterRemove(NET_NDP_ROUTER *p_router,
                                CPU_BOOLEAN    tmr_free);

#if (NET_DBG_CFG_MEM_CLR_EN == DEF_ENABLED)
static void NetNDP_RouterClr(NET_NDP_ROUTER *p_router);
#endif

static NET_NDP_PREFIX *NetNDP_PrefixSrchMatchAddr(NET_IF_NBR          if_nbr,
                                                  const NET_IPv6_ADDR *p_addr,
                                                  RTOS_ERR            *p_err);

static NET_NDP_PREFIX *NetNDP_PrefixCfg(NET_IF_NBR          if_nbr,
                                        const NET_IPv6_ADDR *p_addr_prefix,
                                        CPU_INT08U          prefix_len,
                                        CPU_BOOLEAN         timer_en,
                                        CPU_FNCT_PTR        timeout_fnct,
                                        CPU_INT32U          timeout_ms,
                                        RTOS_ERR            *p_err);

static NET_NDP_PREFIX *NetNDP_PrefixSrch(NET_IF_NBR          if_nbr,
                                         const NET_IPv6_ADDR *p_addr,
                                         RTOS_ERR            *p_err);

static NET_NDP_PREFIX *NetNDP_PrefixGet(RTOS_ERR *p_err);

static void NetNDP_PrefixRemove(NET_NDP_PREFIX *p_prefix,
                                CPU_BOOLEAN    tmr_free);

#if (NET_DBG_CFG_MEM_CLR_EN == DEF_ENABLED)
static void NetNDP_PrefixClr(NET_NDP_PREFIX *p_prefix);
#endif

static NET_NDP_DEST_CACHE *NetNDP_DestCacheCfg(NET_IF_NBR          if_nbr,
                                               const NET_IPv6_ADDR *p_addr_dest,
                                               const NET_IPv6_ADDR *p_addr_next_hop,
                                               CPU_BOOLEAN         is_valid,
                                               CPU_BOOLEAN         on_link,
                                               RTOS_ERR            *p_err);

static NET_NDP_DEST_CACHE *NetNDP_DestCacheGet(RTOS_ERR *p_err);

static NET_NDP_DEST_CACHE *NetNDP_DestCacheSrch(NET_IF_NBR          if_nbr,
                                                const NET_IPv6_ADDR *p_addr);

static NET_NDP_DEST_CACHE *NetNDP_DestCacheSrchInvalid(RTOS_ERR *p_err);

static void NetNDP_DestCacheRemove(NET_NDP_DEST_CACHE *p_dest);

static void NetNDP_DestCacheClr(NET_NDP_DEST_CACHE *p_dest);

static CPU_BOOLEAN NetNDP_IsAddrOnLink(NET_IF_NBR          if_nbr,
                                       const NET_IPv6_ADDR *p_addr);

static void NetNDP_SolicitTimeout(void *p_cache_timeout);

static void NetNDP_ReachableTimeout(void *p_cache_timeout);

static void NetNDP_RouterTimeout(void *p_router_timeout);

static void NetNDP_PrefixTimeout(void *p_prefix_timeout);

#ifdef  NET_DAD_MODULE_EN
static void NetNDP_DAD_Timeout(void *p_cache_timeout);
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                           PUBLIC FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                       NetNDP_CfgNeighborCacheTimeout()
 *
 * @brief    Configure NDP Neighbor timeout from NDP Neighbor cache list.
 *
 * @param    timeout_sec     Desired value for NDP neighbor timeout (in seconds).
 *
 * @return   DEF_OK,   NDP neighbor cache timeout configured.
 *           DEF_FAIL, otherwise.
 *******************************************************************************************************/
CPU_BOOLEAN NetNDP_CfgNeighborCacheTimeout(CPU_INT16U timeout_sec)
{
  CORE_DECLARE_IRQ_STATE;

#if (NET_NDP_CACHE_TIMEOUT_MIN_SEC > DEF_INT_16U_MIN_VAL)
  if (timeout_sec < NET_NDP_CACHE_TIMEOUT_MIN_SEC) {
    return (DEF_FAIL);
  }
#endif
#if (NET_NDP_CACHE_TIMEOUT_MAX_SEC < DEF_INT_16U_MAX_VAL)
  if (timeout_sec > NET_NDP_CACHE_TIMEOUT_MAX_SEC) {
    return (DEF_FAIL);
  }
#endif

  CORE_ENTER_ATOMIC();
  NetNDP_NeighborCacheTimeout_ms = timeout_sec * 1000;
  CORE_EXIT_ATOMIC();

  return (DEF_OK);
}

/****************************************************************************************************//**
 *                                       NetNDP_CfgReachabilityTimeout()
 *
 * @brief    Configure possible NDP Neighbor reachability timeouts.
 *
 * @param    timeout_type    NDP timeout type :
 *                               - NET_NDP_TIMEOUT_REACHABLE
 *                               - NET_NDP_TIMEOUT_DELAY
 *                               - NET_NDP_TIMEOUT_SOLICIT
 *
 * @param    timeout_sec     Desired value for NDP neighbor reachable timeout (in seconds).
 *
 * @return   DEF_OK,   NDP neighbor cache timeout configured.
 *           DEF_FAIL, otherwise.
 *******************************************************************************************************/
CPU_BOOLEAN NetNDP_CfgReachabilityTimeout(NET_NDP_TIMEOUT timeout_type,
                                          CPU_INT16U      timeout_sec)
{
  CORE_DECLARE_IRQ_STATE;

  switch (timeout_type) {
    case NET_NDP_TIMEOUT_REACHABLE:
      if (timeout_sec < NET_NDP_REACHABLE_TIMEOUT_MIN_SEC) {
        return (DEF_FAIL);
      }
      if (timeout_sec > NET_NDP_REACHABLE_TIMEOUT_MAX_SEC) {
        return (DEF_FAIL);
      }
      CORE_ENTER_ATOMIC();
      NetNDP_DataPtr->ReachableTimeout_ms = timeout_sec * 1000;
      CORE_EXIT_ATOMIC();
      break;

    case NET_NDP_TIMEOUT_DELAY:
      if (timeout_sec < NET_NDP_DELAY_FIRST_PROBE_TIMEOUT_MIN_SEC) {
        return (DEF_FAIL);
      }
      if (timeout_sec > NET_NDP_DELAY_FIRST_PROBE_TIMEOUT_MAX_SEC) {
        return (DEF_FAIL);
      }
      CORE_ENTER_ATOMIC();
      NetNDP_DelayTimeout_ms = timeout_sec * 1000;
      CORE_EXIT_ATOMIC();
      break;

    case NET_NDP_TIMEOUT_SOLICIT:
      if (timeout_sec < NET_NDP_RETRANS_TIMEOUT_MIN_SEC) {
        return (DEF_FAIL);
      }
      if (timeout_sec > NET_NDP_RETRANS_TIMEOUT_MAX_SEC) {
        return (DEF_FAIL);
      }
      CORE_ENTER_ATOMIC();
      NetNDP_DataPtr->SolicitTimeout_ms = timeout_sec * 1000;
      CORE_EXIT_ATOMIC();
      break;

    default:
      return (DEF_FAIL);
  }

  return (DEF_OK);
}

/****************************************************************************************************//**
 *                                           NetNDP_CfgSolicitMaxNbr()
 *
 * @brief    Configure NDP maximum number of NDP Solicitation sent for the given type of solicitation.
 *
 * @param    solicit_type    NDP Solicitation message type :
 *                               - NET_NDP_SOLICIT_MULTICAST
 *                               - NET_NDP_SOLICIT_UNICAST
 *                               - NET_NDP_SOLICIT_DAD
 *
 * @param    max_nbr         Desired maximum number of NDP solicitation attempts.
 *
 * @return   DEF_OK,   NDP Request maximum number of solicitation attempts configured.
 *           DEF_FAIL, otherwise.
 *******************************************************************************************************/
CPU_BOOLEAN NetNDP_CfgSolicitMaxNbr(NET_NDP_SOLICIT solicit_type,
                                    CPU_INT08U      max_nbr)
{
  CORE_DECLARE_IRQ_STATE;

  switch (solicit_type) {
    case NET_NDP_SOLICIT_MULTICAST:
#if (NET_NDP_SOLICIT_NBR_MIN > DEF_INT_08U_MIN_VAL)
      if (max_nbr < NET_NDP_SOLICIT_NBR_MIN) {
        return (DEF_FAIL);
      }
#endif
#if (NET_NDP_SOLICIT_NBR_MAX < DEF_INT_08U_MAX_VAL)
      if (max_nbr > NET_NDP_SOLICIT_NBR_MAX) {
        return (DEF_FAIL);
      }
#endif
      CORE_ENTER_ATOMIC();
      NetNDP_DataPtr->SolicitMaxAttempsMulticastNbr = max_nbr;
      CORE_EXIT_ATOMIC();
      break;

    case NET_NDP_SOLICIT_UNICAST:
#if (NET_NDP_SOLICIT_NBR_MIN > DEF_INT_08U_MIN_VAL)
      if (max_nbr < NET_NDP_SOLICIT_NBR_MIN) {
        return (DEF_FAIL);
      }
#endif
#if (NET_NDP_SOLICIT_NBR_MAX < DEF_INT_08U_MAX_VAL)
      if (max_nbr > NET_NDP_SOLICIT_NBR_MAX) {
        return (DEF_FAIL);
      }
#endif
      CORE_ENTER_ATOMIC();
      NetNDP_DataPtr->SolicitMaxAttempsUnicastNbr = max_nbr;
      CORE_EXIT_ATOMIC();
      break;

    case NET_NDP_SOLICIT_DAD:
#ifdef NET_DAD_MODULE_EN
#if (NET_NDP_SOLICIT_NBR_MIN > DEF_INT_08U_MIN_VAL)
      if (max_nbr < NET_NDP_SOLICIT_NBR_MIN) {
        return (DEF_FAIL);
      }
#endif
#if (NET_NDP_SOLICIT_NBR_MAX < DEF_INT_08U_MAX_VAL)
      if (max_nbr > NET_NDP_SOLICIT_NBR_MAX) {
        return (DEF_FAIL);
      }
#endif
      CORE_ENTER_ATOMIC();
      NetNDP_DataPtr->DAD_MaxAttemptsNbr = max_nbr;
      CORE_EXIT_ATOMIC();
#endif
      break;

    default:
      return (DEF_FAIL);
  }

  return (DEF_OK);
}

/****************************************************************************************************//**
 *                                       NetNDP_CfgCacheTxQ_MaxTh()
 *
 * @brief    Configure NDP cache maximum number of transmit packet buffers to enqueue.
 *
 * @param    nbr_buf_max     Desired maximum number of transmit packet buffers to enqueue onto an
 *
 * @return   DEF_OK,   NDP cache transmit packet buffer threshold configured.
 *           DEF_FAIL, otherwise.
 *           Net_InitDflt(),
 *           Application.
 *           This function is a network protocol suite application programming interface (API) function
 *           & MAY be called by application function(s).
 *
 * @note     (1) 'NetNDP_CacheTxQ_MaxTh_nbr' MUST ALWAYS be accessed exclusively in critical sections.
 *******************************************************************************************************/
CPU_BOOLEAN NetNDP_CfgCacheTxQ_MaxTh(NET_BUF_QTY nbr_buf_max)
{
  CORE_DECLARE_IRQ_STATE;

#if (NET_NDP_CACHE_TX_Q_TH_MIN > DEF_INT_16U_MIN_VAL)
  if (nbr_buf_max < NET_NDP_CACHE_TX_Q_TH_MIN) {
    return (DEF_FAIL);
  }
#endif

#if (NET_NDP_CACHE_TX_Q_TH_MAX < DEF_INT_16U_MAX_VAL)
  if (nbr_buf_max > NET_NDP_CACHE_TX_Q_TH_MAX) {
    return (DEF_FAIL);
  }
#endif

  CORE_ENTER_ATOMIC();
  NetNDP_DataPtr->CacheTxQ_MaxTh_nbr = nbr_buf_max;
  CORE_EXIT_ATOMIC();

  return (DEF_OK);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                               NetNDP_Init()
 *
 * @brief    (1) Initialize Neighbor Discovery Protocol Layer :
 *               - (a) Initialize NDP DAD signals.
 *               - (b) Initialize NDP address cache pool.
 *               - (c) Initialize NDP neighbor cache table.
 *               - (d) Initialize NDP address cache.
 *               - (e) Initialize NDP address cache list pointers.
 *               - (f) Initialize NDP router table.
 *               - (g) Initialize NDP router list pointers.
 *               - (h) Initialize NDP prefix table.
 *               - (i) Initialize NDP prefix list pointers.
 *               - (j) Initialize NDP destination cache table.
 *               - (k) Initialize NDP destination cache list pointers.
 *
 * @param    p_err   Pointer to error variable.
 *******************************************************************************************************/
void NetNDP_Init(MEM_SEG  *p_mem_seg,
                 RTOS_ERR *p_err)
{
  NET_NDP_NEIGHBOR_CACHE *p_cache;
  NET_NDP_CACHE_QTY      i;

  NetNDP_DataPtr = (NET_NDP_DATA *)Mem_SegAlloc("NDP Data Segment",
                                                p_mem_seg,
                                                sizeof(NET_NDP_DATA),
                                                p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  //                                                               -------------- CFG NDP INIT DFLT VALS --------------
  (void)NetNDP_CfgNeighborCacheTimeout(NET_NDP_CACHE_TIMEOUT_DFLT_SEC);
  (void)NetNDP_CfgReachabilityTimeout(NET_NDP_TIMEOUT_REACHABLE, NET_NDP_REACHABLE_TIMEOUT_SEC);
  (void)NetNDP_CfgReachabilityTimeout(NET_NDP_TIMEOUT_SOLICIT, NET_NDP_RETRANS_TIMEOUT_SEC);
  (void)NetNDP_CfgReachabilityTimeout(NET_NDP_TIMEOUT_DELAY, NET_NDP_DELAY_FIRST_PROBE_TIMEOUT_SEC);
  (void)NetNDP_CfgSolicitMaxNbr(NET_NDP_SOLICIT_MULTICAST, NET_NDP_SOLICIT_MAX_MULTICAST);
  (void)NetNDP_CfgSolicitMaxNbr(NET_NDP_SOLICIT_UNICAST, NET_NDP_SOLICIT_MAX_UNICAST);
#ifdef  NET_DAD_MODULE_EN
  (void)NetNDP_CfgSolicitMaxNbr(NET_NDP_SOLICIT_DAD, NET_NDP_CFG_DAD_MAX_NBR_ATTEMPTS);
#endif
  (void)NetCache_CfgAccessedTh(NET_CACHE_TYPE_NDP, NET_NDP_CACHE_ACCESSED_TH_DFLT);
  (void)NetNDP_CfgCacheTxQ_MaxTh(NET_NDP_CACHE_TX_Q_TH_DFLT);

  //                                                               ------------ INIT NDP CACHE POOL/STATS -------------
  NetCache_AddrNDP_PoolPtr = DEF_NULL;                          // Init-clr NDP addr. cache pool (see Note #2b).

#if (NET_STAT_POOL_NDP_EN == DEF_ENABLED)
  NetStat_PoolInit(&NetCache_AddrNDP_PoolStat,
                   NET_NDP_CFG_CACHE_NBR);
#endif

  //                                                               ------------ INIT NDP NEIGHBOR CACHE TBL -----------
  p_cache = &NetNDP_NeighborCacheTbl[0];
  for (i = 0u; i < NET_NDP_CFG_CACHE_NBR; i++) {
    p_cache->Type = NET_CACHE_TYPE_NDP;
    p_cache->CacheAddrPtr = &NetCache_AddrNDP_Tbl[i];           // Init each NDP addr cache ptr.
    p_cache->ReqAttemptsCtr = 0u;
    p_cache->State = NET_NDP_CACHE_STATE_NONE;                  // Init each NDP cache as free/NOT used.
    p_cache->Flags = NET_CACHE_FLAG_NONE;
    p_cache->TmrPtr = DEF_NULL;

    NetCache_Init((NET_CACHE_ADDR *)p_cache,                    // Init each NDP addr cache.
                  (NET_CACHE_ADDR *)p_cache->CacheAddrPtr,
                  p_err);
    p_cache++;
  }

  //                                                               ------------- INIT NDP CACHE LIST PTRS -------------
  NetCache_AddrListHead[NET_CACHE_ADDR_LIST_IX_NDP] = DEF_NULL;
  NetCache_AddrListTail[NET_CACHE_ADDR_LIST_IX_NDP] = DEF_NULL;

  NetNDP_DataPtr->DfltRouterTbl = (NET_NDP_ROUTER **)Mem_SegAlloc("Default Router Tbl",
                                                                  p_mem_seg,
                                                                  sizeof(NET_NDP_ROUTER *) * Net_CoreDataPtr->IF_NbrTot,
                                                                  p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

#if (NET_STAT_POOL_NDP_EN == DEF_ENABLED)
  //                                                               ------------ INIT NDP ROUTER POOL/STATS -------------
  NetStat_PoolInit(&NetNDP_DataPtr->RouterPoolStat,
                   NET_NDP_CFG_ROUTER_NBR);
#endif

  //                                                               ----------------- INIT ROUTER LIST -----------------
  Mem_DynPoolCreate("NDP Router Pool",
                    &NetNDP_DataPtr->RouterPool,
                    p_mem_seg,
                    sizeof(NET_NDP_ROUTER),
                    sizeof (CPU_ALIGN),
                    1,
                    NET_NDP_CFG_ROUTER_NBR,
                    p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  SList_Init(&NetNDP_DataPtr->RouterListPtr);

#if (NET_STAT_POOL_NDP_EN == DEF_ENABLED)
  //                                                               ------------ INIT NDP PREFIX POOL/STATS ------------
  NetStat_PoolInit(&NetNDP_DataPtr->PrefixPoolStat,
                   NET_NDP_CFG_PREFIX_NBR);
#endif

  //                                                               ----------------- INIT PREFIX LIST -----------------
  Mem_DynPoolCreate("NDP Prefix Pool",
                    &NetNDP_DataPtr->PrefixPool,
                    p_mem_seg,
                    sizeof(NET_NDP_PREFIX),
                    sizeof (CPU_ALIGN),
                    1,
                    NET_NDP_CFG_PREFIX_NBR,
                    p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  SList_Init(&NetNDP_DataPtr->PrefixListPtr);

#if (NET_STAT_POOL_NDP_EN == DEF_ENABLED)
  //                                                               --------- INIT NDP DESTINATION POOL/STATS ----------
  NetStat_PoolInit(&NetNDP_DataPtr->DestPoolStat,
                   NET_NDP_CFG_DEST_NBR);
#endif

  //                                                               -------------- INIT DESTINATION LIST ---------------
  Mem_DynPoolCreate("NDP Destination Pool",
                    &NetNDP_DataPtr->DestPool,
                    p_mem_seg,
                    sizeof(NET_NDP_DEST_CACHE),
                    sizeof (CPU_ALIGN),
                    1,
                    NET_NDP_CFG_DEST_NBR,
                    p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  SList_Init(&NetNDP_DataPtr->DestListPtr);
}

/****************************************************************************************************//**
 *                                       NetNDP_NeighborCacheHandler()
 *
 * @brief    - (1) Resolve destination hardware address using NDP :
 *               - (a) Search NDP Cache List for NDP cache with corresponding protocol address.
 *               - (b) If NDP cache found, handle packet based on NDP cache state :
 *                   - (1) INCOMPLETE  -> Enqueue transmit packet buffer to NDP cache
 *                   - (2) REACHABLE   -> Copy NDP cache's hardware address to data packet;
 *                                        Return to Network Interface to transmit data packet
 *               - (c) If NDP cache NOT found, allocate new NDP cache in 'INCOMPLETE' state (see Note #1b1)
 *                     See 'net_cache.h  CACHE STATES' for cache state diagram.
 *           - (2) This NDP cache handler function assumes the following :
 *               - (a) ALL NDP caches in the NDP Cache List are valid. [validated by NetCache_AddrGet()]
 *               - (b) ANY NDP cache in the 'INCOMPLETE' state MAY have already enqueued at LEAST one
 *                     transmit packet buffer when NDP cache allocated. [see NetCache_AddrGet()]
 *               - (c) ALL NDP caches in the 'REACHABLE' state have valid hardware addresses.
 *               - (d) ALL transmit buffers enqueued on any NDP cache are valid.
 *               - (e) Buffer's NDP address pointers pre-configured by Network Interface to point to :
 *                   - (1) 'NDP_AddrProtocolPtr'               Pointer to the protocol address used to
 *                                                                   resolve the hardware address
 *                   - (2) 'NDP_AddrHW_Ptr'                    Pointer to memory buffer to return the
 *                                                                   resolved hardware address
 *                   - (3) NDP addresses                       Which MUST be in network-order
 *
 * @param    p_buf   Pointer to network buffer to transmit.
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function.
 *
 * @note     (3) Since the primary tasks of the network protocol suite are prevented from running
 *               concurrently (see 'net.h  Note #3'), it is NOT necessary to protect the shared
 *               resources of the NDP Cache List since no asynchronous access from other network
 *               tasks is possible.
 *
 * @note     (4) See RFC #4861, Section 7.2.2.
 *
 * @note     (5) Some buffer controls were previously initialized in NetBuf_Get() when the packet
 *               was received at the network interface layer. These buffer controls do NOT need
 *               to be re-initialized but are shown for completeness.
 *
 * @note     (6) A resolved multicast address still remains resolved even if any error(s) occur
 *               while adding it to the NDP cache.
 *******************************************************************************************************/
NET_CACHE_STATE NetNDP_NeighborCacheHandler(const NET_BUF *p_buf,
                                            RTOS_ERR      *p_err)
{
#ifdef  NET_MCAST_MODULE_EN
  NET_IF_NBR        if_nbr;
  NET_PROTOCOL_TYPE protocol_type_net;
  CPU_BOOLEAN       addr_protocol_multicast;
  CPU_INT08U        addr_hw_len;
#endif
  NET_BUF_HDR            *p_buf_hdr;
  CPU_INT08U             *p_addr_hw;
  CPU_INT08U             *p_addr_protocol;
  NET_BUF_HDR            *p_tail_buf_hdr;
  NET_BUF                *p_tail_buf;
  NET_CACHE_ADDR_NDP     *p_cache_addr_ndp;
  NET_ICMPv6_HDR         *p_icmp_hdr;
  NET_NDP_NEIGHBOR_CACHE *p_cache;
  CPU_INT32U             timeout_ms;
  NET_CACHE_STATE        cache_state = NET_CACHE_STATE_NONE;
  NET_IP_ADDR_FAMILY     ip_type;
  NET_BUF_QTY            buf_max_th;
  CORE_DECLARE_IRQ_STATE;

  p_buf_hdr = (NET_BUF_HDR *)&p_buf->Hdr;
  p_addr_hw = p_buf_hdr->NDP_AddrHW_Ptr;
  p_addr_protocol = p_buf_hdr->NDP_AddrProtocolPtr;

  if_nbr = p_buf_hdr->IF_Nbr;

  //                                                               ------- VALIDATE IF PACKET IS OF ICMPv6 TYPE -------
  if (p_buf_hdr->ICMP_MsgIx != NET_BUF_IX_NONE) {
    p_icmp_hdr = (NET_ICMPv6_HDR *)&p_buf->DataPtr[p_buf_hdr->ICMP_MsgIx];
  } else {
    p_icmp_hdr = DEF_NULL;
  }

  //                                                               ------------------ SRCH NDP CACHE ------------------
  p_cache_addr_ndp = (NET_CACHE_ADDR_NDP *)NetCache_AddrSrch(NET_CACHE_TYPE_NDP,
                                                             if_nbr,
                                                             p_addr_protocol,
                                                             NET_IPv6_ADDR_SIZE);

  if (p_cache_addr_ndp != DEF_NULL) {                           // If NDP cache found, chk state.
    p_cache = (NET_NDP_NEIGHBOR_CACHE *)p_cache_addr_ndp->ParentPtr;
    switch (p_cache->State) {
      case NET_NDP_CACHE_STATE_INCOMPLETE:                      // If NDP cache pend, append buf into Q (see Note #4).
        CORE_ENTER_ATOMIC();
        buf_max_th = NetNDP_DataPtr->CacheTxQ_MaxTh_nbr;
        CORE_EXIT_ATOMIC();

        if (p_cache_addr_ndp->TxQ_Nbr >= buf_max_th) {
          RTOS_ERR_SET(*p_err, RTOS_ERR_NET_ADDR_UNRESOLVED);
          goto exit;
        }

        p_tail_buf = p_cache_addr_ndp->TxQ_Tail;
        if (p_tail_buf != DEF_NULL) {                           // If Q NOT empty,    append buf @ Q tail.
          p_tail_buf_hdr = &p_tail_buf->Hdr;
          p_tail_buf_hdr->NextSecListPtr = (NET_BUF *)p_buf;
          p_buf_hdr->PrevSecListPtr = (NET_BUF *)p_tail_buf;
          p_cache_addr_ndp->TxQ_Tail = (NET_BUF *)p_buf;
        } else {                                                // Else add buf as first q'd buf.
          p_cache_addr_ndp->TxQ_Head = (NET_BUF *)p_buf;
          p_cache_addr_ndp->TxQ_Tail = (NET_BUF *)p_buf;
#if 0                                                           // Init'd in NetBuf_Get() [see Note #5].
          p_buf_hdr->PrevSecListPtr = DEF_NULL;
          p_buf_hdr->NextSecListPtr = DEF_NULL;
#endif
        }

        p_cache_addr_ndp->TxQ_Nbr++;
        //                                                         Cfg buf's unlink fnct/obj to NDP cache.
        p_buf_hdr->UnlinkFnctPtr = (NET_BUF_FNCT)&NetCache_UnlinkBuf;
        p_buf_hdr->UnlinkObjPtr = (void *) p_cache_addr_ndp;

        cache_state = NET_CACHE_STATE_PEND;
        break;

      case NET_NDP_CACHE_STATE_REACHABLE:                       // If NDP cache REACHABLE, copy hw addr.
        Mem_Copy(p_addr_hw,
                 p_cache_addr_ndp->AddrHW,
                 NET_IF_HW_ADDR_LEN_MAX);
        cache_state = NET_CACHE_STATE_RESOLVED;
        break;

      case NET_NDP_CACHE_STATE_STALE:
      case NET_NDP_CACHE_STATE_DLY:
        if (p_icmp_hdr != DEF_NULL) {
          if (p_icmp_hdr->Type != NET_ICMPv6_MSG_TYPE_NDP_NEIGHBOR_ADV) {
            p_cache->State = NET_NDP_CACHE_STATE_DLY;
            CORE_ENTER_ATOMIC();
            timeout_ms = NetNDP_DelayTimeout_ms;
            CORE_EXIT_ATOMIC();
            NetTmr_Set(p_cache->TmrPtr,
                       NetNDP_DelayTimeout,
                       timeout_ms);
          }
        }
        Mem_Copy(p_addr_hw,
                 p_cache_addr_ndp->AddrHW,
                 NET_IF_HW_ADDR_LEN_MAX);
        cache_state = NET_CACHE_STATE_RESOLVED;
        break;

      case NET_NDP_CACHE_STATE_PROBE:
        Mem_Copy(p_addr_hw,
                 p_cache_addr_ndp->AddrHW,
                 NET_IF_HW_ADDR_LEN_MAX);
        cache_state = NET_CACHE_STATE_RESOLVED;
        break;

      default:
        NetNDP_NeighborCacheRemoveEntry(p_cache, DEF_YES);
        NetNDP_NeighborCacheAddPend(p_buf, p_buf_hdr, p_addr_protocol, p_err);
        if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
          goto exit;
        }
        cache_state = NET_CACHE_STATE_PEND;
        break;
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
                                                    NET_IPv6_ADDR_SIZE);

    if (addr_protocol_multicast == DEF_YES) {                   // If multicast protocol addr,      ...
      addr_hw_len = NET_IF_HW_ADDR_LEN_MAX;
      //                                                           ... convert to multicast hw addr ...
      NetIF_AddrMulticastProtocolToHW(if_nbr,
                                      p_addr_protocol,
                                      NET_IPv6_ADDR_SIZE,
                                      protocol_type_net,
                                      p_addr_hw,
                                      &addr_hw_len,
                                      p_err);
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        goto exit;
      }
      //                                                           Rtn resolved multicast hw addr (see Note #6).
      cache_state = NET_CACHE_STATE_RESOLVED;
      goto exit;
    }
#endif

    NetNDP_NeighborCacheAddPend(p_buf, p_buf_hdr, p_addr_protocol, p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      goto exit;
    }

    cache_state = NET_CACHE_STATE_PEND;
  }

exit:
  return (cache_state);
}

/****************************************************************************************************//**
 *                                               NetNDP_Rx()
 *
 * @brief    (1) Process received NDP packets :
 *               - (a) Demultiplex received ICMPv6 packet according to the ICMPv6/NDP Type.
 *               - (b) Update receive statistics
 *
 * @param    p_buf       Pointer to network buffer that received the NDP packet.
 *
 * @param    p_buf_hdr   Pointer to network buffer header.
 *
 * @param    p_icmp_hdr  Pointer to received packet's ICMP header.
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function.
 *******************************************************************************************************/
void NetNDP_Rx(const NET_BUF        *p_buf,
               NET_BUF_HDR          *p_buf_hdr,
               const NET_ICMPv6_HDR *p_icmp_hdr,
               RTOS_ERR             *p_err)
{
  switch (p_icmp_hdr->Type) {
    case NET_ICMPv6_MSG_TYPE_NDP_ROUTER_ADV:
      NET_CTR_STAT_INC(Net_StatCtrs.NDP.RxMsgAdvRouterCtr);
      NetNDP_RxRouterAdvertisement(p_buf,
                                   p_buf_hdr,
                                   (NET_NDP_ROUTER_ADV_HDR *)p_icmp_hdr,
                                   p_err);
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        goto exit;
      }
      break;

    case NET_ICMPv6_MSG_TYPE_NDP_NEIGHBOR_SOL:
      NET_CTR_STAT_INC(Net_StatCtrs.NDP.RxMsgSolNborCtr);
      NetNDP_RxNeighborSolicitation(p_buf,
                                    p_buf_hdr,
                                    (NET_NDP_NEIGHBOR_SOL_HDR *)p_icmp_hdr,
                                    p_err);
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        goto exit;
      }
      break;

    case NET_ICMPv6_MSG_TYPE_NDP_NEIGHBOR_ADV:
      NET_CTR_STAT_INC(Net_StatCtrs.NDP.RxMsgAdvNborCtr);
      NetNDP_RxNeighborAdvertisement(p_buf,
                                     p_buf_hdr,
                                     (const NET_NDP_NEIGHBOR_ADV_HDR *)p_icmp_hdr,
                                     p_err);
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        goto exit;
      }
      break;

    case NET_ICMPv6_MSG_TYPE_NDP_REDIRECT:
      NET_CTR_STAT_INC(Net_StatCtrs.NDP.RxMsgRedirectCtr);
      NetNDP_RxRedirect(p_buf,
                        p_buf_hdr,
                        (NET_NDP_REDIRECT_HDR *)p_icmp_hdr,
                        p_err);
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        goto exit;
      }
      break;

    default:
      NET_CTR_ERR_INC(Net_ErrCtrs.NDP.InvTypeCtr);
      RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
      goto exit;
  }

exit:
  return;
}

/****************************************************************************************************//**
 *                                       NetNDP_TxRouterSolicitation()
 *
 * @brief    (1) Transmit router solicitation message:
 *               - (a) Set IPv6 source      address. See Note #2.
 *               - (b) Set IPv6 destination address as the multicast all-routers address.
 *               - (c) Transmit ICMP message.
 *
 * @param    if_nbr      Network interface number to transmit Router Solicitation message.
 *
 * @param    p_addr_src  Pointer to IPv6 source address (see Note #1).
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function.
 *
 * @note     (2) If IPv6 source address pointer is NULL, the unspecified address will be used.
 *******************************************************************************************************/
void NetNDP_TxRouterSolicitation(NET_IF_NBR    if_nbr,
                                 NET_IPv6_ADDR *p_addr_src,
                                 RTOS_ERR      *p_err)
{
  NET_IPv6_ADDR addr_all_routers_mcast;
  NET_IPv6_ADDR addr_unspecified;
  NET_IPv6_ADDR *p_ndp_addr_src;

  //                                                               ---------------- SET SOURCE ADDRESS ----------------
  p_ndp_addr_src = p_addr_src;
  if (p_ndp_addr_src == DEF_NULL) {
    NetIPv6_AddrUnspecifiedSet(&addr_unspecified);
    p_ndp_addr_src = &addr_unspecified;
  }

  //                                                               -------------- SET DESTINATION ADDRESS -------------
  NetIPv6_AddrMcastAllRoutersSet(&addr_all_routers_mcast, DEF_NO);

  //                                                               ------------ TX NDP ROUTER SOLICITATION ------------
  //                                                               Tx router solicitation.
  (void)NetICMPv6_TxMsgReqHandler(if_nbr,
                                  NET_ICMPv6_MSG_TYPE_NDP_ROUTER_SOL,
                                  NET_ICMPv6_MSG_CODE_NDP_ROUTER_SOL,
                                  0u,
                                  p_ndp_addr_src,
                                  &addr_all_routers_mcast,
                                  NET_IPv6_HDR_HOP_LIM_MAX,
                                  DEF_NO,
                                  DEF_NULL,
                                  DEF_NULL,
                                  0u,
                                  p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

exit:
  return;
}

/****************************************************************************************************//**
 *                                           NetNDP_NextHopByIF()
 *
 * @brief    Find Next Hop for the given destination address and interface.
 *           Add new Destination cache if none present.
 *
 * @param    if_nbr          Interface number on which packet will be send.
 *
 * @param    p_addr_dest     Pointer to IPv6 destination address.
 *
 * @param    p_addr_nexthop  Pointer to variable that will receive the next hop IPv6 address.
 *
 * @param    p_err           Pointer to variable that will receive the return error code from this function.
 *
 * @return   Pointer to the next-hop ipv6 address
 *
 * @note     (1) The interface number given is assumed to be the good interface to used to reach
 *               the destination.
 *
 * @note     (2) Use function NetNDP_NextHop() when interface is unknown.
 *******************************************************************************************************/
NET_IP_TX_DEST_STATUS NetNDP_NextHopByIF(NET_IF_NBR          if_nbr,
                                         const NET_IPv6_ADDR *p_addr_dest,
                                         NET_IPv6_ADDR       **p_addr_nexthop,
                                         RTOS_ERR            *p_err)
{
  NET_NDP_DEST_CACHE    *p_dest_cache;
  NET_NDP_ROUTER        *p_router;
  CPU_BOOLEAN           on_link;
  CPU_BOOLEAN           addr_mcast;
  CPU_BOOLEAN           dflt_router;
  NET_IP_TX_DEST_STATUS dest_status = NET_IP_TX_DEST_STATUS_NONE;

  //                                                               --------- CHECK IF DEST ADDR IS MULTICAST ----------
  addr_mcast = NetIPv6_IsAddrMcast(p_addr_dest);
  if (addr_mcast == DEF_YES) {
    *p_addr_nexthop = (NET_IPv6_ADDR *)p_addr_dest;
    dest_status = NET_IP_TX_DEST_STATUS_MULTICAST;
    goto exit;
  }

  //                                                               -------- CHECK FOR DESTINATION CACHE ENTRY ---------
  p_dest_cache = NetNDP_DestCacheSrch(if_nbr, p_addr_dest);
  if (p_dest_cache != DEF_NULL) {                               // Destination cache exists for current destination.
    if (p_dest_cache->IsValid == DEF_YES) {
      *p_addr_nexthop = &p_dest_cache->AddrNextHop;
      if (p_dest_cache->OnLink == DEF_YES) {
        dest_status = NET_IP_TX_DEST_STATUS_LOCAL_NET;
      } else {
        dest_status = NET_IP_TX_DEST_STATUS_DFLT_GATEWAY;
      }
      goto exit;
    }
  } else {
    p_dest_cache = NetNDP_DestCacheCfg(if_nbr,
                                       p_addr_dest,
                                       DEF_NULL,
                                       DEF_NO,
                                       DEF_NO,
                                       p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      *p_addr_nexthop = DEF_NULL;
      goto exit;
    }
  }
  //                                                               ---------- CHECK IF DESTINAION IS ON LINK ----------
  on_link = NetNDP_IsAddrOnLink(if_nbr,
                                p_addr_dest);
  if (on_link == DEF_YES) {
    p_dest_cache->IsValid = DEF_YES;
    p_dest_cache->OnLink = DEF_YES;
    Mem_Copy(&p_dest_cache->AddrNextHop, p_addr_dest, NET_IPv6_ADDR_SIZE);
    dest_status = NET_IP_TX_DEST_STATUS_LOCAL_NET;
    *p_addr_nexthop = (NET_IPv6_ADDR *)p_addr_dest;
    goto exit;
  }

  //                                                               ------------- CHECK FOR ROUTER ON LINK -------------
  dflt_router = NetNDP_RouterDfltGet(if_nbr, &p_router, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    p_dest_cache->IsValid = DEF_NO;
    p_dest_cache->OnLink = DEF_NO;
    *p_addr_nexthop = DEF_NULL;
    RTOS_ERR_SET(*p_err, RTOS_ERR_NET_NEXT_HOP);
    goto exit;
  }

  if (dflt_router == DEF_YES) {
    p_dest_cache->IsValid = DEF_YES;
    p_dest_cache->OnLink = DEF_NO;
    dest_status = NET_IP_TX_DEST_STATUS_DFLT_GATEWAY;
  } else {
    p_dest_cache->IsValid = DEF_NO;
    p_dest_cache->OnLink = DEF_NO;
    dest_status = NET_IP_TX_DEST_STATUS_NO_DFLT_GATEWAY;
  }

  *p_addr_nexthop = (NET_IPv6_ADDR *)&p_router->Addr;
  Mem_Copy(&p_dest_cache->AddrNextHop, *p_addr_nexthop, NET_IPv6_ADDR_SIZE);

exit:
  return (dest_status);
}

/****************************************************************************************************//**
 *                                               NetNDP_NextHop()
 *
 * @brief    Find Next Hop and the best Interface for the given destination address.
 *
 * @param    p_if_nbr        Pointer to variable that will received the interface number.
 *
 * @param    p_addr_src      Pointer to IPv6 suggested source address if any.
 *
 * @param    p_addr_dest     Pointer to IPv6 destination address.
 *
 * @param    p_addr_nexthop  Pointer to variable that will received the next-hop ipv6 address.
 *
 * @param    p_err           Pointer to variable that will receive the return error code from this function.
 *
 * @return   Destination status
 *
 * @note     (1) Destination address that are Link-Local are ambiguous, no interface can be chosen
 *               as being adequate for the destination since link local prefix is the same for any
 *               network. Therefore the default inteface is selected.
 *
 * @note     (2) To found the Interface for a Multicast destination address, the function checks
 *               if any interface as joined the MLDP group of the address.
 *
 * @note     (3) This function finds the best matching interface for the given destination address
 *               according to the following classification :
 *               - (a) Destination is on same link as the interface and the received source address is
 *                     also configured on the interface.
 *               - (b) Destination is on same link as the interface.
 *               - (c) A Default router exit on the same link as the interface and the received source is
 *                     also configured on the interface.
 *               - (d) A default router exit on the same link as the interface.
 *               - (e) A router is present on the same link as the interface and the received source
 *                     address is also configured on the interface.
 *               - (f) The received source address is configured on the interface.
 *               - (g) No Interface is adequate for the destination address.
 *******************************************************************************************************/
NET_IP_TX_DEST_STATUS NetNDP_NextHop(NET_IF_NBR          *p_if_nbr,
                                     const NET_IPv6_ADDR *p_addr_src,
                                     const NET_IPv6_ADDR *p_addr_dest,
                                     NET_IPv6_ADDR       **p_addr_nexthop,
                                     RTOS_ERR            *p_err)
{
  NET_IF_NBR            if_nbr;
  NET_NDP_DEST_CACHE    *p_dest = DEF_NULL;
  NET_NDP_DEST_CACHE    *p_dest_tmp;
  NET_NDP_ROUTER        *p_router;
  NET_IPv6_ADDR         *p_addr_nexthop_tmp;
  NET_IF_NBR            if_nbr_tmp;
  NET_IF_NBR            if_nbr_src_addr = NET_IF_NBR_NONE;
  CPU_INT08U            valid_nbr_current;
  CPU_INT08U            valid_nbr_best;
  CPU_BOOLEAN           link_local;
  CPU_BOOLEAN           addr_mcast;
  CPU_BOOLEAN           on_link;
  CPU_BOOLEAN           add_dest = DEF_NO;
  CPU_BOOLEAN           add_dest_tmp;
  CPU_BOOLEAN           dflt_router;
  CPU_BOOLEAN           is_mcast_grp;
  CPU_BOOLEAN           if_found;
  NET_IP_TX_DEST_STATUS dest_status = NET_IP_TX_DEST_STATUS_NONE;

  //                                                               --------- CHECK IF DEST ADDR IS LINK LOCAL ---------
  link_local = NetIPv6_IsAddrLinkLocal(p_addr_dest);
  if (link_local == DEF_YES) {
    *p_if_nbr = NetIF_GetDflt();
    dest_status = NetNDP_NextHopByIF(*p_if_nbr, p_addr_dest, p_addr_nexthop, p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      goto exit;
    }
    goto exit;
  }

  //                                                               --------- CHECK IF DEST ADDR IS MULTICAST ----------
  addr_mcast = NetIPv6_IsAddrMcast(p_addr_dest);
  if (addr_mcast == DEF_YES) {
    if_nbr = NET_IF_NBR_BASE_CFGD;
    if_nbr_tmp = NET_IF_NBR_BASE_CFGD;
    if_found = DEF_NO;
    for (; if_nbr_tmp < Net_CoreDataPtr->IF_NbrCfgd; if_nbr_tmp++) {
      is_mcast_grp = NetMLDP_IsGrpJoinedOnIF(if_nbr_tmp, p_addr_dest);
      if (is_mcast_grp == DEF_YES) {
        if_found = DEF_YES;
        if_nbr = if_nbr_tmp;
      }
    }
    if (if_found == DEF_YES) {
      *p_if_nbr = if_nbr;
      dest_status = NET_IP_TX_DEST_STATUS_MULTICAST;
    } else {
      *p_if_nbr = NET_IF_NBR_NONE;
      RTOS_ERR_SET(*p_err, RTOS_ERR_NET_NEXT_HOP);
    }
    *p_addr_nexthop = (NET_IPv6_ADDR *)p_addr_dest;
    goto exit;
  }

  //                                                               ----------- FOUND BEST NEXT HOP ADDRESS ------------
  if (p_addr_src != DEF_NULL) {
    if_nbr_src_addr = NetIPv6_GetAddrHostIF_Nbr(p_addr_src);
  }

  if_nbr = NET_IF_NBR_BASE_CFGD;
  if_nbr_tmp = NET_IF_NBR_BASE_CFGD;
  valid_nbr_best = 0;
  valid_nbr_current = 0;
  for (; if_nbr_tmp < Net_CoreDataPtr->IF_NbrCfgd; if_nbr_tmp++) {
    //                                                             -------------- CHECK DESTINATION CACHE -------------
    p_dest_tmp = NetNDP_DestCacheSrch(if_nbr_tmp, p_addr_dest);
    //                                                             Destination cache exists for destination ...
    //                                                             ... and destination cache is valid.
    if ((p_dest_tmp != DEF_NULL)
        && (p_dest_tmp->IsValid == DEF_YES) ) {
      p_addr_nexthop_tmp = &p_dest_tmp->AddrNextHop;

      if (p_dest_tmp->OnLink == DEF_YES) {
        valid_nbr_current = NET_NDP_IF_DEST_ON_LINK;
      } else {
        valid_nbr_current = NET_NDP_IF_DFLT_ROUTER_ON_LINK;
      }

      add_dest_tmp = DEF_NO;

      //                                                           Destination cache doesn't exists for destination ...
      //                                                           ... or is invalid.
    } else {
      if (p_dest_tmp == DEF_NULL) {
        add_dest_tmp = DEF_YES;
      } else {
        add_dest_tmp = DEF_NO;
      }

      //                                                           --------- CHECK IF DESTINATION IS ON LINK ----------
      on_link = NetNDP_IsAddrOnLink(if_nbr_tmp,
                                    p_addr_dest);
      if (on_link == DEF_YES) {
        valid_nbr_current = NET_NDP_IF_DEST_ON_LINK;
        p_addr_nexthop_tmp = (NET_IPv6_ADDR *)p_addr_dest;
      } else {
        //                                                         ------------ CHECK FOR ROUTER ON LINK --------------
        if (NetNDP_DataPtr->DfltRouterTbl[if_nbr_tmp] == DEF_NULL) {
          dflt_router = NetNDP_RouterDfltGet(if_nbr_tmp, &p_router, p_err);
          if (p_router != DEF_NULL) {
            if (dflt_router == DEF_YES) {
              valid_nbr_current = NET_NDP_IF_DFLT_ROUTER_ON_LINK;
            } else {
              valid_nbr_current = NET_NDP_IF_ROUTER_ON_LINK;
            }
            p_addr_nexthop_tmp = &p_router->Addr;
          } else {
            p_addr_nexthop_tmp = DEF_NULL;
            RTOS_ERR_SET(*p_err, RTOS_ERR_NET_NEXT_HOP);
          }
        } else {
          valid_nbr_current = NET_NDP_IF_DFLT_ROUTER_ON_LINK;
          p_addr_nexthop_tmp = &NetNDP_DataPtr->DfltRouterTbl[if_nbr_tmp]->Addr;
          dest_status = NET_IP_TX_DEST_STATUS_DFLT_GATEWAY;
        }
      }
    }

    if (if_nbr_src_addr == if_nbr_tmp) {
      valid_nbr_current++;
    }

    if (valid_nbr_current >= valid_nbr_best) {
      valid_nbr_best = valid_nbr_current;
      if_nbr = if_nbr_tmp;
      *p_addr_nexthop = p_addr_nexthop_tmp;
      add_dest = add_dest_tmp;
      if (add_dest == DEF_NO) {
        p_dest = p_dest_tmp;
      }
    }
  }

  //                                                               Add a Destination Cache if none exist for dest.
  if (add_dest == DEF_YES) {
    p_dest = NetNDP_DestCacheCfg(if_nbr,
                                 p_addr_dest,
                                 *p_addr_nexthop,
                                 DEF_NO,
                                 DEF_NO,
                                 p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      *p_addr_nexthop = DEF_NULL;
      goto exit;
    }
  }

  //                                                               Set return status.
  switch (valid_nbr_best) {
    case NET_NDP_IF_DEST_ON_LINK_WITH_SRC_ADDR_CFGD:
    case NET_NDP_IF_DEST_ON_LINK:
      p_dest->IsValid = DEF_YES;
      p_dest->OnLink = DEF_YES;
      dest_status = NET_IP_TX_DEST_STATUS_LOCAL_NET;
      break;

    case NET_NDP_IF_DFLT_ROUTER_ON_LINK_WITH_SRC_ADDR_CFGD:
    case NET_NDP_IF_DFLT_ROUTER_ON_LINK:
      p_dest->IsValid = DEF_YES;
      p_dest->OnLink = DEF_NO;
      dest_status = NET_IP_TX_DEST_STATUS_DFLT_GATEWAY;
      break;

    case NET_NDP_IF_ROUTER_ON_LINK_WITH_SRC_ADDR_CFGD:
    case NET_NDP_IF_ROUTER_ON_LINK:
      p_dest->IsValid = DEF_NO;
      p_dest->OnLink = DEF_NO;
      dest_status = NET_IP_TX_DEST_STATUS_NO_DFLT_GATEWAY;
      break;

    case NET_NDP_IF_SRC_ADDR_CFGD:
    case NET_NDP_IF_NO_MATCH:
    default:
      p_dest->IsValid = DEF_NO;
      p_dest->OnLink = DEF_NO;
      RTOS_ERR_SET(*p_err, RTOS_ERR_NET_NEXT_HOP);
      goto exit;
  }

  *p_if_nbr = if_nbr;

exit:
  return (dest_status);
}

/****************************************************************************************************//**
 *                                           NetNDP_CacheTimeout()
 *
 * @brief    Discard an NDP cache in the 'STALE' state on timeout.
 *
 * @param    p_cache_timeout     Pointer to an NDP cache (see Note #2b).
 *
 * @note     (1) RFC #4861 section 7.3.3 Node Behavior.
 *
 * @note     (2) This function is a network timer callback function :
 *               - (a) Clear the timer pointer ... :
 *                   - (1) Cleared in NetCache_AddrFree() via NetCache_Remove().
 *               - (b) but do NOT re-free the timer.
 *******************************************************************************************************/
void NetNDP_CacheTimeout(void *p_cache_timeout)
{
  NET_NDP_NEIGHBOR_CACHE *p_cache;
  NET_CACHE_ADDR_NDP     *p_cache_addr_ndp;
  NET_NDP_ROUTER         *p_router;
  CPU_BOOLEAN            is_router;
  CPU_INT32U             timeout_ms;
  RTOS_ERR               local_err;
  CORE_DECLARE_IRQ_STATE;

  p_cache = (NET_NDP_NEIGHBOR_CACHE *)p_cache_timeout;          // See Note #2b2A.

  p_cache_addr_ndp = p_cache->CacheAddrPtr;

  p_cache->TmrPtr = DEF_NULL;                                   // Un-reference tmr in the NDP Neighbor cache.

  is_router = DEF_BIT_IS_SET(p_cache->Flags, NET_NDP_CACHE_FLAG_ISROUTER);

  if (is_router == DEF_TRUE) {                                  // If the addr is still a valid router don't delete it.
    RTOS_ERR_SET(local_err, RTOS_ERR_NONE);

    p_router = NetNDP_RouterSrch(p_cache_addr_ndp->IF_Nbr,
                                 (NET_IPv6_ADDR *)&p_cache_addr_ndp->AddrProtocol[0],
                                 &local_err);

    if (p_router != DEF_NULL) {
      CORE_ENTER_ATOMIC();
      timeout_ms = p_router->LifetimeSec * 1000;
      CORE_EXIT_ATOMIC();

      RTOS_ERR_SET(local_err, RTOS_ERR_NONE);

      p_cache->TmrPtr = NetTmr_Get(NetNDP_CacheTimeout,
                                   p_cache,
                                   timeout_ms,
                                   NET_TMR_OPT_NONE,
                                   &local_err);
      if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {      // If tmr unavailable, free NDP cache.
        NetNDP_NeighborCacheRemoveEntry(p_cache, DEF_NO);
        goto exit;
      }
    } else {
      NetNDP_NeighborCacheRemoveEntry(p_cache, DEF_NO);
    }
  } else {
    NetNDP_NeighborCacheRemoveEntry(p_cache, DEF_NO);
  }

exit:
  return;
}

/****************************************************************************************************//**
 *                                           NetNDP_DelayTimeout()
 *
 * @brief    Change the NDP cache state to PROBE if the state of the NDP cache is still at DELAY when
 *           the timer end.
 *
 * @param    p_cache_timeout     Pointer to an NDP cache.
 *
 * @note     (1) This function is a network timer callback function :
 *               - (a) Clear the timer pointer ... :
 *                     - (1) Cleared in NetNDP_CacheFree() via NetNDP_CacheRemove(); or
 *                     - (2) Reset   by NetTmr_Get().
 *               - (b) but do NOT re-free the timer.
 *******************************************************************************************************/
void NetNDP_DelayTimeout(void *p_cache_timeout)
{
  NET_NDP_NEIGHBOR_CACHE *p_cache;
  CPU_INT32U             timeout_ms;
  RTOS_ERR               local_err;
  CORE_DECLARE_IRQ_STATE;

  RTOS_ERR_SET(local_err, RTOS_ERR_NONE);

  p_cache = (NET_NDP_NEIGHBOR_CACHE *)p_cache_timeout;

  p_cache->TmrPtr = DEF_NULL;                                   // Deference already freed timer.

  if (p_cache->State == NET_NDP_CACHE_STATE_DLY) {
    p_cache->State = NET_NDP_CACHE_STATE_PROBE;
    CORE_ENTER_ATOMIC();
    timeout_ms = NetNDP_DataPtr->SolicitTimeout_ms;
    CORE_EXIT_ATOMIC();

    p_cache->TmrPtr = NetTmr_Get(NetNDP_SolicitTimeout,
                                 p_cache,
                                 timeout_ms,
                                 NET_TMR_OPT_NONE,
                                 &local_err);
    if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {        // If tmr unavail, free NDP cache.
      NetNDP_NeighborCacheRemoveEntry(p_cache, DEF_NO);
      goto exit;
    }
  }

exit:
  return;
}

/****************************************************************************************************//**
 *                                           NetNDP_DAD_Start()
 *
 * @brief    (1) Start duplicate address detection procedure:
 *               - (a) Use a new cache entry to save address info.
 *
 * @param    if_nbr  Network interface number to perform duplicate address detection.
 *
 * @param    p_addr  Pointer on the IPv6 addr to perform duplicate address detection.
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function.
 *
 * @return   DEF_OK,   if duplicate address detection started successfully,
 *           DEF_FAIL, otherwise.
 *
 * @note     (2) NetNDP_DAD_Start() is called by network protocol suite function(s) &
 *               MUST be called with the global network lock already acquired.
 *******************************************************************************************************/
#ifdef NET_DAD_MODULE_EN
void NetNDP_DAD_Start(NET_IF_NBR    if_nbr,
                      NET_IPv6_ADDR *p_addr,
                      RTOS_ERR      *p_err)
{
  CPU_INT32U timeout_ms;
  CORE_DECLARE_IRQ_STATE;

  //                                                               ---------- CREATE NEW NDP CACHE FOR ADDR -----------
  CORE_ENTER_ATOMIC();
  timeout_ms = NetNDP_DataPtr->SolicitTimeout_ms;
  CORE_EXIT_ATOMIC();

  (void)NetNDP_NeighborCacheAddEntry(if_nbr,
                                     DEF_NULL,
                                     (CPU_INT08U *)p_addr,
                                     DEF_NULL,
                                     timeout_ms,
                                     NetNDP_DAD_Timeout,
                                     NET_NDP_CACHE_STATE_INCOMPLETE,
                                     DEF_NO,
                                     p_err);
}
#endif

/****************************************************************************************************//**
 *                                               NetNDP_DAD_Stop()
 *
 * @brief    Remove the NDP Neighbor cache entry associated with the DAD process.
 *
 * @param    if_nbr  Interface number of the address on which DAD is occurring.
 *
 * @param    p_addr  Pointer to the IPv6 address on which DAD is occurring.
 *
 * @note     (1) NetNDP_DAD_Stop() is called by network protocol suite function(s) &
 *               MUST be called with the global network lock already acquired.
 *******************************************************************************************************/
#ifdef NET_DAD_MODULE_EN
void NetNDP_DAD_Stop(NET_IF_NBR    if_nbr,
                     NET_IPv6_ADDR *p_addr)
{
  NET_CACHE_ADDR_NDP     *p_cache_addr_ndp;
  NET_NDP_NEIGHBOR_CACHE *p_cache;

  p_cache_addr_ndp = (NET_CACHE_ADDR_NDP *)NetCache_AddrSrch(NET_CACHE_TYPE_NDP,
                                                             if_nbr,
                                                             (CPU_INT08U *)p_addr,
                                                             NET_IPv6_ADDR_SIZE);
  if (p_cache_addr_ndp == DEF_NULL) {
    return;
  }

  p_cache = (NET_NDP_NEIGHBOR_CACHE *)p_cache_addr_ndp->ParentPtr;

  NetNDP_NeighborCacheRemoveEntry(p_cache, DEF_YES);
}
#endif

/****************************************************************************************************//**
 *                                       NetNDP_DAD_GetMaxAttemptsNbr()
 *
 * @brief    Get the number of DAD attempts configured.
 *
 * @return   Number of DAD attempts configured.
 *
 * @note     (1) NetNDP_DAD_GetMaxAttemptsNbr() is called by network protocol suite function(s) &
 *               MUST be called with the global network lock already acquired.
 *******************************************************************************************************/
#ifdef NET_DAD_MODULE_EN
CPU_INT08U NetNDP_DAD_GetMaxAttemptsNbr(void)
{
  CPU_INT08U dad_retx_nbr;
  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_ATOMIC();
  dad_retx_nbr = NetNDP_DataPtr->DAD_MaxAttemptsNbr;
  CORE_EXIT_ATOMIC();

  return(dad_retx_nbr);
}
#endif

/****************************************************************************************************//**
 *                                       NetNDP_RouterAdvSignalCreate()
 *
 * @brief    Create Signal for Rx Router Advertisement message.
 *
 * @param    if_nbr  Network Interface number.
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function.
 *
 * @return   Pointer to Signal handle for the given interface.
 *******************************************************************************************************/
#ifdef  NET_IPv6_ADDR_AUTO_CFG_MODULE_EN
KAL_SEM_HANDLE *NetNDP_RouterAdvSignalCreate(NET_IF_NBR if_nbr,
                                             RTOS_ERR   *p_err)
{
  NET_IF         *p_if = DEF_NULL;
  KAL_SEM_HANDLE *p_signal = DEF_NULL;

  p_if = NetIF_Get(if_nbr, p_err);                              // retrieve Interface object.
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  p_signal = &p_if->IP_Obj->IPv6_NDP_RxRouterAdvSignal;
  *p_signal = KAL_SemCreate(NET_NDP_RX_ROUTER_ADV_SIGNAL_NAME,
                            DEF_NULL,
                            p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

exit:
  return (p_signal);
}
#endif

/****************************************************************************************************//**
 *                                       NetNDP_RouterAdvSignalPend()
 *
 * @brief    Wait for Rx Router Advertisement Signal.
 *
 * @param    p_signal    Pointer to signal handle.
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function.
 *******************************************************************************************************/
#ifdef  NET_IPv6_ADDR_AUTO_CFG_MODULE_EN
void NetNDP_RouterAdvSignalPend(KAL_SEM_HANDLE *p_signal,
                                RTOS_ERR       *p_err)
{
  KAL_SemPend(*p_signal,
              KAL_OPT_PEND_BLOCKING,
              NET_NDP_RX_ROUTER_ADV_TIMEOUT_MS,
              p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_CODE_GET(*p_err));
  }
}
#endif

/****************************************************************************************************//**
 *                                       NetNDP_RouterAdvSignalRemove()
 *
 * @brief    Delete given Router Adv Signal.
 *
 * @param    p_signal    Pointer to signal handle.
 *******************************************************************************************************/
#ifdef  NET_IPv6_ADDR_AUTO_CFG_MODULE_EN
void NetNDP_RouterAdvSignalRemove(KAL_SEM_HANDLE *p_signal)
{
  KAL_SemDel(*p_signal);

  p_signal->SemObjPtr = DEF_NULL;
}
#endif

/****************************************************************************************************//**
 *                                           NetNDP_PrefixAddCfg()
 *
 * @brief    Add IPv6 prefix configuration in the prefix pool.
 *
 * @param    if_nbr          Interface number.
 *
 * @param    p_addr_prefix   Pointer to IPv6 prefix.
 *
 * @param    prefix_len      Prefix length
 *
 * @param    timer_en        Indicate whether are not to set a network timer for the prefix:
 *                           DEF_YES                Set network timer for prefix.
 *                           DEF_NO          Do NOT set network timer for prefix.
 *
 * @param    timeout_fnct    Pointer to timeout function.
 *
 * @param    timeout_ms      Timeout value (in milliseconds).
 *
 * @param    p_err           Pointer to variable that will receive the return error code from this function:
 *
 * @return   Pointer to configured prefix entry.
 *******************************************************************************************************/
NET_NDP_PREFIX *NetNDP_PrefixAddCfg(NET_IF_NBR          if_nbr,
                                    const NET_IPv6_ADDR *p_addr_prefix,
                                    CPU_INT08U          prefix_len,
                                    CPU_BOOLEAN         timer_en,
                                    CPU_FNCT_PTR        timeout_fnct,
                                    CPU_INT32U          timeout_ms,
                                    RTOS_ERR            *p_err)
{
  NET_NDP_PREFIX *p_prefix = DEF_NULL;

  p_prefix = NetNDP_PrefixSrch(if_nbr,
                               p_addr_prefix,
                               p_err);
  if (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE) {
    return (DEF_NULL);
  }

  NetNDP_PrefixCfg(if_nbr,
                   p_addr_prefix,
                   prefix_len,
                   timer_en,
                   timeout_fnct,
                   timeout_ms,
                   p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (DEF_NULL);
  }

  return (p_prefix);
}

/****************************************************************************************************//**
 *                                           NetNDP_DestCacheAddCfg()
 *
 * @brief    Add IPv6 NDP Destination cache configuration in the Destination Cache pool.
 *
 * @param    if_nbr              Interface number for the destination to configure.
 *
 * @param    p_addr_dest         Pointer to IPv6 Destination address.
 *
 * @param    p_addr_next_hop     Pointer to Next-Hop IPv6 address.
 *
 * @param    is_valid            Indicate whether are not the Next-Hop address is valid.
 *                               DEF_YES, address is   valid
 *                               DEF_NO,  address is invalid
 *
 * @param    on_link             Indicate whether are not the Destination is on link.
 *                               DEF_YES, destination is     on link
 *                               DEF_NO,  destination is not on link
 *
 * @param    p_err               Pointer to variable that will receive the return error code from this function.
 *
 * @return   Pointer to destination entry configured.
 *
 * @note     (1) Function for testing purpose.
 *******************************************************************************************************/
NET_NDP_DEST_CACHE *NetNDP_DestCacheAddCfg(NET_IF_NBR          if_nbr,
                                           const NET_IPv6_ADDR *p_addr_dest,
                                           const NET_IPv6_ADDR *p_addr_next_hop,
                                           CPU_BOOLEAN         is_valid,
                                           CPU_BOOLEAN         on_link,
                                           RTOS_ERR            *p_err)
{
  NET_NDP_DEST_CACHE *p_dest = DEF_NULL;
  CPU_BOOLEAN        is_cfgd = DEF_NO;

  Net_GlobalLockAcquire((void *)NetNDP_DestCacheAddCfg);

  //                                                               ----------------- VALIDATE IF NBR ------------------
  is_cfgd = NetIF_IsValidCfgdHandler(if_nbr);
  if (is_cfgd != DEF_YES) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_HANDLE);
    goto exit_release;
  }

  p_dest = NetNDP_DestCacheSrch(if_nbr,
                                p_addr_dest);
  if (p_dest != DEF_NULL) {
    goto exit_release;
  }

  NetNDP_DestCacheCfg(if_nbr,
                      p_addr_dest,
                      DEF_NULL,
                      DEF_YES,
                      DEF_NO,
                      p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit_release;
  }

  PP_UNUSED_PARAM(p_addr_next_hop);
  PP_UNUSED_PARAM(is_valid);
  PP_UNUSED_PARAM(on_link);

exit_release:
  Net_GlobalLockRelease();

  return (p_dest);
}

/****************************************************************************************************//**
 *                                       NetNDP_DestCacheRemoveCfg()
 *
 * @brief    Add IPv6 NDP Destination cache configuration in the Destination Cache pool.
 *
 * @param    if_nbr          Interface number for the destination to configure.
 *
 * @param    p_addr_dest     Pointer to IPv6 Destination address.
 *
 * @param    p_err           Pointer to variable that will receive the return error code from this function.
 *
 * @note     (1) Function for testing purpose.
 *******************************************************************************************************/
void NetNDP_DestCacheRemoveCfg(NET_IF_NBR          if_nbr,
                               const NET_IPv6_ADDR *p_addr_dest,
                               RTOS_ERR            *p_err)
{
  NET_NDP_DEST_CACHE *p_dest = DEF_NULL;

  Net_GlobalLockAcquire((void *)NetNDP_DestCacheAddCfg);

  p_dest = NetNDP_DestCacheSrch(if_nbr,
                                p_addr_dest);
  if (p_dest == DEF_NULL) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_FOUND);
    goto exit_release;
  }

  NetNDP_DestCacheRemove(p_dest);

exit_release:
  Net_GlobalLockRelease();
}

/****************************************************************************************************//**
 *                                           NetNDP_CacheClrAll()
 *
 * @brief    Clear all entries of the NDP cache, Router list, Prefix list and Destination cache.
 *
 * @note     (1) Function for testing purpose.
 *******************************************************************************************************/
void NetNDP_CacheClrAll(void)
{
  NET_NDP_NEIGHBOR_CACHE *p_cache;
  NET_CACHE_ADDR_NDP     *p_cache_addr_ndp;
  NET_NDP_ROUTER         *p_router;
  NET_NDP_PREFIX         *p_prefix;
  NET_NDP_DEST_CACHE     *p_dest;
  CPU_INT08U             i;

  //                                                               ----------------- ACQUIRE NET LOCK -----------------
  Net_GlobalLockAcquire((void *)NetNDP_CacheClrAll);

  //                                                               Clear NDP Addr Cache and Neighbor Cache.
  p_cache_addr_ndp = (NET_CACHE_ADDR_NDP *)NetCache_AddrListHead[NET_CACHE_ADDR_LIST_IX_NDP];
  while (p_cache_addr_ndp != DEF_NULL) {
    NetNDP_RemoveAddrDestCache(p_cache_addr_ndp->IF_Nbr,
                               &p_cache_addr_ndp->AddrProtocol[0]);

    p_cache = (NET_NDP_NEIGHBOR_CACHE *)p_cache_addr_ndp->ParentPtr;
    p_cache->ReqAttemptsCtr = 0u;
    p_cache->State = NET_NDP_CACHE_STATE_NONE;
    p_cache->Flags = NET_CACHE_FLAG_NONE;

    NetCache_Remove((NET_CACHE_ADDR *)p_cache_addr_ndp,         // Clr Addr Cache and free tmr if specified.
                    DEF_YES);

    p_cache->TmrPtr = DEF_NULL;
    p_cache_addr_ndp = (NET_CACHE_ADDR_NDP *)NetCache_AddrListHead[NET_CACHE_ADDR_LIST_IX_NDP];
  }

  //                                                               Clear Default router list.
  while (NetNDP_DataPtr->RouterListPtr != DEF_NULL) {
    p_router = SLIST_ENTRY(NetNDP_DataPtr->RouterListPtr, NET_NDP_ROUTER, ListNode);
    NetNDP_RouterRemove(p_router, DEF_YES);
  }

  for (i = 0; i < Net_CoreDataPtr->IF_NbrTot; i++) {
    NetNDP_DataPtr->DfltRouterTbl[i] = DEF_NULL;
  }

  //                                                               Clear Prefix list.
  while (NetNDP_DataPtr->PrefixListPtr != DEF_NULL) {
    p_prefix = SLIST_ENTRY(NetNDP_DataPtr->PrefixListPtr, NET_NDP_PREFIX, ListNode);
    NetNDP_PrefixRemove(p_prefix, DEF_YES);
  }

  //                                                               Clear Destination Cache.
  while (NetNDP_DataPtr->DestListPtr != DEF_NULL) {
    p_dest = SLIST_ENTRY(NetNDP_DataPtr->DestListPtr, NET_NDP_DEST_CACHE, ListNode);
    NetNDP_DestCacheRemove(p_dest);
  }

  //                                                               ----------------- RELEASE NET LOCK -----------------
  Net_GlobalLockRelease();
}

/****************************************************************************************************//**
 *                                           NetNDP_CacheGetState()
 *
 * @brief    Retrieve the cache state of the NDP neighbor cache entry related to the interface and
 *           address received.
 *
 * @param    if_nbr  Interface number
 *
 * @param    p_addr  Pointer to IPv6 address of the neighbor
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function.
 *
 * @return   Cache state of the Neighbor cache entry.
 *
 * @note     (1) Function for testing purpose.
 *******************************************************************************************************/
CPU_INT08U NetNDP_CacheGetState(NET_IF_NBR          if_nbr,
                                const NET_IPv6_ADDR *p_addr,
                                RTOS_ERR            *p_err)
{
  NET_CACHE_ADDR_NDP     *p_cache_addr_ndp;
  NET_NDP_NEIGHBOR_CACHE *p_cache;
  CPU_INT08U             state = NET_NDP_CACHE_STATE_NONE;

  Net_GlobalLockAcquire((void *)NetNDP_CacheGetState);

  p_cache_addr_ndp = (NET_CACHE_ADDR_NDP *)NetCache_AddrSrch(NET_CACHE_TYPE_NDP,
                                                             if_nbr,
                                                             (CPU_INT08U *)p_addr,
                                                             NET_IPv6_ADDR_SIZE);
  if (p_cache_addr_ndp == DEF_NULL) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_FOUND);
    goto exit_release;
  }

  p_cache = (NET_NDP_NEIGHBOR_CACHE *)p_cache_addr_ndp->ParentPtr;
  state = p_cache->State;

exit_release:
  //                                                               ----------------- RELEASE NET LOCK -----------------
  Net_GlobalLockRelease();

  return (state);
}

/****************************************************************************************************//**
 *                                   NetNDP_CacheGetIsRouterFlagState()
 *
 * @brief    Retrieve the IsRouter flag state for the given neighbor cache related to the interface
 *           and address received.
 *
 * @param    if_nbr  Interface number
 *
 * @param    p_addr  Pointer to IPv6 address of the neighbor
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function.
 *
 * @return   IsRouter flag state : DEF_YES, neighbor is also a router
 *                                 DEF_NO , neighbor has not advertise itself as a router
 *
 * @note     (1) Function for testing purpose.
 *******************************************************************************************************/
CPU_BOOLEAN NetNDP_CacheGetIsRouterFlagState(NET_IF_NBR          if_nbr,
                                             const NET_IPv6_ADDR *p_addr,
                                             RTOS_ERR            *p_err)
{
  NET_CACHE_ADDR_NDP     *p_cache_addr_ndp;
  NET_NDP_NEIGHBOR_CACHE *p_cache;
  CPU_BOOLEAN            is_router;

  is_router = DEF_NO;

  Net_GlobalLockAcquire((void *)NetNDP_CacheGetState);

  p_cache_addr_ndp = (NET_CACHE_ADDR_NDP *)NetCache_AddrSrch(NET_CACHE_TYPE_NDP,
                                                             if_nbr,
                                                             (CPU_INT08U *)p_addr,
                                                             NET_IPv6_ADDR_SIZE);
  if (p_cache_addr_ndp == DEF_NULL) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_FOUND);
    goto exit_release;
  }

  p_cache = (NET_NDP_NEIGHBOR_CACHE *) p_cache_addr_ndp->ParentPtr;
  is_router = DEF_BIT_IS_SET(p_cache->Flags, NET_NDP_CACHE_FLAG_ISROUTER);

exit_release:
  //                                                               ----------------- RELEASE NET LOCK -----------------
  Net_GlobalLockRelease();

  return (is_router);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                       NetNDP_RxRouterAdvertisement()
 *
 * @brief    Receive Router Advertisement message.
 *
 * @param    p_buf       Pointer to network buffer that received ICMP packet.
 *
 * @param    p_buf_hdr   Pointer to network buffer header.
 *
 * @param    p_ndp_hdr   Pointer to received packet's NDP header.
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function.
 *
 * @note     (1) See RFC #4861 Section 6.1.2, Section 6.3.4 Processing Received Router Advertisements,
 *                       and Section 7.3.3.
 *
 * @note     (2) See NetNDP_RxPrefixUpdate() function description for details on handling Rx Prefix
 *                       Information.
 *
 * @note     (3) Additional processing of Rx NDP Router Advertisement may be needed to implement in the future
 *                       #### NET-793
 *******************************************************************************************************/
static void NetNDP_RxRouterAdvertisement(const NET_BUF                *p_buf,
                                         NET_BUF_HDR                  *p_buf_hdr,
                                         const NET_NDP_ROUTER_ADV_HDR *p_ndp_hdr,
                                         RTOS_ERR                     *p_err)
{
  NET_IF_NBR                  if_nbr;
  NET_NDP_OPT_TYPE            opt_type;
  NET_NDP_OPT_LEN             opt_len;
  CPU_INT16U                  opt_len_tot;
  CPU_INT16U                  opt_len_cnt;
  CPU_INT08U                  *p_ndp_opt;
  NET_NDP_ROUTER              *p_router;
  NET_CACHE_ADDR_NDP          *p_cache_addr_ndp;
  NET_NDP_NEIGHBOR_CACHE      *p_cache_ndp;
  NET_NDP_OPT_HDR             *p_ndp_opt_hdr;
  NET_NDP_OPT_HW_ADDR_HDR     *p_ndp_opt_hw_addr_hdr;
  NET_NDP_OPT_MTU_HDR         *p_ndp_opt_mtu_hdr;
  NET_NDP_OPT_PREFIX_INFO_HDR *p_ndp_opt_prefix_info_hdr;
  NET_IPv6_ADDR               *p_addr_prefix;
  NET_IPv6_HDR                *p_ip_hdr;
  NET_IF                      *p_if;
  CPU_INT32U                  timeout_ms;
  CPU_INT32U                  router_mtu;
  CPU_INT16U                  router_lifetime_sec;
  CPU_INT32U                  retx_timeout;
  CPU_INT32U                  lifetime_valid;
  CPU_INT32U                  lifetime_preferred;
  CPU_INT08U                  prefix_len;
  CPU_INT08U                  hop_limit_ip;
  CPU_INT08U                  hw_addr[NET_IP_HW_ADDR_LEN];
  CPU_INT08U                  hw_addr_len;
  CPU_BOOLEAN                 addr_link_local;
  CPU_BOOLEAN                 addr_cfg_auto;
  CPU_BOOLEAN                 on_link;
  CPU_BOOLEAN                 hw_addr_this_host;
  CPU_BOOLEAN                 opt_type_valid;
  CPU_BOOLEAN                 opt_type_src_addr;
  CPU_BOOLEAN                 addr_unspecified;
  CPU_BOOLEAN                 prefix_Mcast;
  CPU_BOOLEAN                 prefix_link_local;
  CPU_BOOLEAN                 addr_cfg_other;
  CPU_BOOLEAN                 addr_cfg_managed;
  CORE_DECLARE_IRQ_STATE;

  addr_link_local = NetIPv6_IsAddrLinkLocal(&p_buf_hdr->IPv6_AddrSrc);
  if (addr_link_local == DEF_NO) {
    NET_CTR_ERR_INC(Net_ErrCtrs.NDP.RxInvalidAddrSrc);
    RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
    goto exit;
  }

  p_ip_hdr = (NET_IPv6_HDR *)&p_buf->DataPtr[p_buf_hdr->IP_HdrIx];
  hop_limit_ip = p_ip_hdr->HopLim;
  if (hop_limit_ip != NET_IPv6_HDR_HOP_LIM_MAX) {
    NET_CTR_ERR_INC(Net_ErrCtrs.NDP.RxInvalidHopLimit);
    RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
    goto exit;
  }

  addr_unspecified = NetIPv6_IsAddrUnspecified(&p_buf_hdr->IPv6_AddrSrc);
  if (addr_unspecified == DEF_TRUE) {
    NET_CTR_ERR_INC(Net_ErrCtrs.NDP.RxInvalidAddrSrc);
    RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
    goto exit;
  }

  //                                                               DHCPv6 Flags
  addr_cfg_managed = DEF_BIT_IS_SET(p_ndp_hdr->Flags, NET_NDP_HDR_FLAG_ADDR_CFG_MNGD);
  addr_cfg_other = DEF_BIT_IS_SET(p_ndp_hdr->Flags, NET_NDP_HDR_FLAG_ADDR_CFG_OTHER);

  PP_UNUSED_PARAM(addr_cfg_managed);
  PP_UNUSED_PARAM(addr_cfg_other);

  if_nbr = p_buf_hdr->IF_Nbr;

  NET_UTIL_VAL_COPY_GET_NET_16(&router_lifetime_sec, &p_ndp_hdr->RouterLifetime);

  timeout_ms = router_lifetime_sec * 1000;

  //                                                               ---- UPDATE ROUTER ENTRY IN DEFAULT ROUTER LIST ----
  //                                                               Search in Router List for Address.
  p_router = NetNDP_RouterSrch(if_nbr,
                               &p_buf_hdr->IPv6_AddrSrc,
                               p_err);

  if ((p_router == DEF_NULL)                                    // Router address is not in Default Router List.
      && (router_lifetime_sec != 0)) {
    p_router = NetNDP_RouterCfg(if_nbr,
                                &p_buf_hdr->IPv6_AddrSrc,
                                DEF_YES,
                                NetNDP_RouterTimeout,
                                timeout_ms,
                                p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      goto exit;
    }

    p_router->LifetimeSec = router_lifetime_sec;
  } else if ((p_router != DEF_NULL)                             // Router addr is already in list and...
             && (router_lifetime_sec != 0)) {                   // ... router lifetime is non-zero.
    if (p_router->TmrPtr != DEF_NULL) {
      //                                                           Update Router Lifetime tmr.
      NetTmr_Set(p_router->TmrPtr,
                 NetNDP_RouterTimeout,
                 timeout_ms);

      p_router->LifetimeSec = router_lifetime_sec;
    }
  } else if ((p_router != DEF_NULL)                             // Router addr is already in list and...
             && (router_lifetime_sec == 0)) {                   // ... router lifetime = 0.
    NetNDP_RemoveAddrDestCache(p_router->IF_Nbr,
                               &p_router->Addr.Addr[0]);
    NetNDP_RouterRemove(p_router, DEF_YES);                     // Remove router from router list.
  } else {                                                      // Router addr is not in router list and ...
                                                                // ... lifetime = 0.
    NET_CTR_ERR_INC(Net_ErrCtrs.NDP.RxInvalidRouterAdvCtr);
    RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_FOUND);
    goto exit;
  }

  //                                                               ----------- UPDATE NDP RE-TRANSMIT TIMER -----------
  NET_UTIL_VAL_COPY_GET_NET_32(&retx_timeout, &p_ndp_hdr->ReTxTmr);
  retx_timeout = retx_timeout / NET_NDP_MS_NBR_PER_SEC;
  (void)NetNDP_CfgReachabilityTimeout(NET_NDP_TIMEOUT_SOLICIT,
                                      (CPU_INT16U) retx_timeout);

  //                                                               ------- SEARCH NEIGHBOR CACHE FOR ROUTER ADDR ------
  p_cache_addr_ndp = (NET_CACHE_ADDR_NDP *)NetCache_AddrSrch(NET_CACHE_TYPE_NDP,
                                                             if_nbr,
                                                             (CPU_INT08U *)&p_buf_hdr->IPv6_AddrSrc,
                                                             NET_IPv6_ADDR_SIZE);

  //                                                               ------------- SCAN RA FOR VALID OPTIONS ------------
  opt_len_tot = p_buf_hdr->IP_TotLen - sizeof(NET_NDP_ROUTER_ADV_HDR) + sizeof(CPU_INT32U);
  p_ndp_opt = (CPU_INT08U *)&p_ndp_hdr->Opt;

  opt_len_cnt = 0u;
  opt_type_valid = DEF_NO;
  opt_type_src_addr = DEF_NO;

  while ((p_ndp_opt != DEF_NULL)
         && (*p_ndp_opt != NET_NDP_OPT_TYPE_NONE)
         && (opt_len_cnt < opt_len_tot)) {
    p_ndp_opt_hdr = (NET_NDP_OPT_HDR *)p_ndp_opt;
    opt_type = p_ndp_opt_hdr->Type;
    opt_len = p_ndp_opt_hdr->Len;

    if (opt_len == 0u) {
      NET_CTR_ERR_INC(Net_ErrCtrs.NDP.RxInvalidOptLen);
      RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
      goto exit;
    }

    switch (opt_type) {
      case NET_NDP_OPT_TYPE_ADDR_SRC:
        opt_type_valid = DEF_YES;
        opt_type_src_addr = DEF_YES;
        p_ndp_opt_hw_addr_hdr = (NET_NDP_OPT_HW_ADDR_HDR *) p_ndp_opt;
        hw_addr_len = (opt_len * DEF_OCTET_NBR_BITS) - (NET_NDP_OPT_DATA_OFFSET);
        if (hw_addr_len != NET_IF_HW_ADDR_LEN_MAX) {
          NET_CTR_ERR_INC(Net_ErrCtrs.NDP.RxInvalidAddrLen);
          RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
          goto exit;
        }

        hw_addr_len = sizeof(hw_addr);
        NetIF_AddrHW_GetHandler(if_nbr, hw_addr, &hw_addr_len, p_err);
        if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
          goto exit;
        }

        hw_addr_this_host = Mem_Cmp(&hw_addr[0],
                                    &p_ndp_opt_hw_addr_hdr->Addr[0],
                                    hw_addr_len);
        if (hw_addr_this_host == DEF_TRUE) {
          NET_CTR_ERR_INC(Net_ErrCtrs.NDP.RxAddrHWThisHost);
          RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
          goto exit;
        }

        //                                                         Neighbor cache entry exists for the router.
        if (p_cache_addr_ndp != DEF_NULL) {
          NetNDP_NeighborCacheUpdateEntry(p_cache_addr_ndp,
                                          &p_ndp_opt_hw_addr_hdr->Addr[0]);

          p_cache_ndp = (NET_NDP_NEIGHBOR_CACHE *)p_cache_addr_ndp->ParentPtr;
          DEF_BIT_SET(p_cache_ndp->Flags, NET_NDP_CACHE_FLAG_ISROUTER);

          if (p_router != DEF_NULL) {
            p_router->NDP_CachePtr = p_cache_ndp;
          }
          //                                                       No neighbor cache entry exits for the router.
        } else {
          CORE_ENTER_ATOMIC();
          timeout_ms = NetNDP_NeighborCacheTimeout_ms;
          CORE_EXIT_ATOMIC();

          p_cache_addr_ndp = NetNDP_NeighborCacheAddEntry(if_nbr,
                                                          (CPU_INT08U *)&p_ndp_opt_hw_addr_hdr->Addr[0],
                                                          (CPU_INT08U *)&p_buf_hdr->IPv6_AddrSrc,
                                                          DEF_NULL,
                                                          timeout_ms,
                                                          &NetNDP_CacheTimeout,
                                                          NET_NDP_CACHE_STATE_STALE,
                                                          DEF_YES,
                                                          p_err);

          p_cache_ndp = (NET_NDP_NEIGHBOR_CACHE *)p_cache_addr_ndp->ParentPtr;

          if (p_router != DEF_NULL) {
            p_router->NDP_CachePtr = p_cache_ndp;
          }
        }
        break;

      case NET_NDP_OPT_TYPE_MTU:
        opt_type_valid = DEF_YES;
        p_ndp_opt_mtu_hdr = (NET_NDP_OPT_MTU_HDR *) p_ndp_opt;
        NET_UTIL_VAL_COPY_GET_NET_32(&router_mtu, &p_ndp_opt_mtu_hdr->MTU);

        p_if = NetIF_Get(if_nbr, p_err);
        RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );

        if ((router_mtu >= NET_IPv6_MAX_DATAGRAM_SIZE_DFLT)
            && (router_mtu <= p_if->MTU)               ) {      // TODO_NET: Need to have new interface API in IF for max MTU get.
          NetIF_MTU_SetHandler(if_nbr, router_mtu, p_err);
        }
        break;

      case NET_NDP_OPT_TYPE_PREFIX_INFO:
        opt_type_valid = DEF_YES;

        if (opt_len != CPU_WORD_SIZE_32) {                      // Prefix Information option must be 32 bytes.
          NET_CTR_ERR_INC(Net_ErrCtrs.NDP.RxInvalidOptType);
          RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
          break;
        }

        p_ndp_opt_prefix_info_hdr = (NET_NDP_OPT_PREFIX_INFO_HDR *)p_ndp_opt;

        p_addr_prefix = &p_ndp_opt_prefix_info_hdr->Prefix;
        prefix_len = p_ndp_opt_prefix_info_hdr->PrefixLen;

        if (prefix_len > NET_NDP_PREFIX_LEN_MAX) {              // Nbr of valid bits of the prefix cannot exceed 128.
          NET_CTR_ERR_INC(Net_ErrCtrs.NDP.RxInvalidOptType);
          RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
          break;
        }

        prefix_Mcast = NetIPv6_IsAddrMcast(p_addr_prefix);
        prefix_link_local = NetIPv6_IsAddrLinkLocal(p_addr_prefix);

        if ((prefix_Mcast == DEF_YES)                           // Prefix must not have a link-local scope and ...
            || (prefix_link_local == DEF_YES)) {                // ... must not be a multicast addr prefix.
          NET_CTR_ERR_INC(Net_ErrCtrs.NDP.RxInvalidOptType);
          RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
          break;
        }

        on_link = DEF_BIT_IS_SET(p_ndp_opt_prefix_info_hdr->Flags, NET_NDP_HDR_FLAG_ON_LINK);
        addr_cfg_auto = DEF_BIT_IS_SET(p_ndp_opt_prefix_info_hdr->Flags, NET_NDP_HDR_FLAG_ADDR_CFG_AUTO);

        NET_UTIL_VAL_COPY_GET_NET_32(&lifetime_valid, &p_ndp_opt_prefix_info_hdr->ValidLifetime);
        NET_UTIL_VAL_COPY_GET_NET_32(&lifetime_preferred, &p_ndp_opt_prefix_info_hdr->PreferredLifetime);

        NetNDP_RxPrefixUpdate(if_nbr,
                              p_addr_prefix,
                              prefix_len,
                              on_link,
                              addr_cfg_auto,
                              lifetime_valid,
                              lifetime_preferred,
                              p_err);
        if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
          goto exit;
        }
        break;

      case NET_NDP_OPT_TYPE_ADDR_TARGET:
      case NET_NDP_OPT_TYPE_REDIRECT:
        break;

      default:
        NET_CTR_ERR_INC(Net_ErrCtrs.NDP.RxInvalidOptType);
        RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
        goto exit;
    }

    p_ndp_opt += (opt_len * DEF_OCTET_NBR_BITS);
    opt_len_cnt += (opt_len * DEF_OCTET_NBR_BITS);
  }

  if ((opt_type_src_addr == DEF_NO)
      && (p_cache_addr_ndp != DEF_NULL)) {
    p_cache_ndp = (NET_NDP_NEIGHBOR_CACHE *)p_cache_addr_ndp->ParentPtr;
    DEF_BIT_SET(p_cache_ndp->Flags, NET_NDP_CACHE_FLAG_ISROUTER);
  }

  if ((opt_len_tot > 0)
      && (opt_type_valid == DEF_NO)) {
    NET_CTR_ERR_INC(Net_ErrCtrs.NDP.RxInvalidOptType);
    RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
    goto exit;
  }

  (void)NetNDP_UpdateDefaultRouter(if_nbr);

exit:
  return;
}

/****************************************************************************************************//**
 *                                       NetNDP_RxNeighborSolicitation()
 *
 * @brief    Receive Neighbor Solicitation message.
 *
 * @param    p_buf       Pointer to network buffer that received NDP packet.
 *
 * @param    p_buf_hdr   Pointer to network buffer header.
 *
 * @param    p_ndp_hdr   Pointer to received packet's NDP header.
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function.
 *
 * @note     (1) RFC #4861, Section 7.1.1 states that "A node MUST silently discard any received
 *                       Neighbor Solicitation messages that does not satisfy all of the following validity
 *                       checks":
 *
 *                           - (a) The IP Hop Limit field has a value of 255.
 *
 *                           - (b) ICMP checksum     is valid.
 *
 *                           - (c) ICMP code         is zero.
 *
 *                           - (d) ICMP length (derived from the IP length) is 24 or more octets.
 *
 *                           - (e) Target Address is not a multicast address.
 *
 *                           - (f) All included options have a length that is greater than zero.
 *
 *                           - (g) If the IP source address is the unspecified address, the IP destination
 *                       address is a solicited-node multicast address,
 *
 *                           - (h) If the IP source address is the unspecified address, there is no source
 *                       link-layer address option in the message.
 *
 * @note     (2) RFC #4861, Section 7.2.3 details the receipt of a Neighbor Solicitation Message :
 *
 *                           - (a) "A valid Neighbor Solicitation that does not meet any of the following
 *                       requirements MUST be silently discarded:"
 *
 *                           - (1) "The Target Address is a "valid" unicast or anycast address assigned to
 *                       the receiving interface,"
 *
 *                           - (2) "The Target Address is a unicast or anycast address for which the node is
 *                       offering proxy service, or"
 *
 *                           - (3) "The Target Address is a "tentative" address on which Duplicate Address
 *                       Detection is being performed."
 *
 *                           - (b) "If the Target Address is tentative, the Neighbor Solicitation should be
 *                       processed as described in RFC #4862 section 5.4 :"
 *
 *                           - (1) "If the target address is tentative, and the source address is a unicast
 *                       address, the solicitation's sender is performing address resolution on the
 *                       target; the solicitation should be silently ignored."
 *
 *                           - (2) "Otherwise, processing takes place as described below. In all cases, a node
 *                       MUST NOT respond to a Neighbor Solicitation for a tentative address."
 *
 *                           - (A) "If the source address of the Neighbor Solicitation is the unspecified
 *                       address, the solicitation is from a node performing Duplicate Address
 *                       Detection.
 *
 *                           - (1) "If the solicitation is from another node, the tentative address is
 *                       a duplicate and should not be used (by either node)."
 *
 *                           - (2) "If the solicitation is from the node itself (because the node loops
 *                       back multicast packets), the solicitation does not indicate the
 *                       presence of a duplicate address."
 *
 *                           - (c) "If the Source Address is not the unspecified address and, on link layers that
 *                       have addresses, the solicitation includes a Source Link-Layer Address option,
 *                       then the recipient SHOULD create or update the Neighbor Cache entry for the IP
 *                       Source Address of the solicitation."
 *
 *                           - (d) "If an entry does not already exist, the node SHOULD create a new one and set
 *                       its reachability state to STALE."
 *
 *                           - (e) "If an entry already exists, and the cached link-layer address differs from the
 *                       one in the received Source Link-Layer option, the cached address should be
 *                       replaced by the received address, and the entry's reachability state MUST be
 *                       set to STALE."
 *
 *                           - (f) "If a Neighbor Cache entry is created, the IsRouter flag SHOULD be set to FALSE."
 *
 *                           - (g) "If a Neighbor Cache entry already exists, its IsRouter flag MUST NOT be
 *                       modified."
 *
 *                           - (h) "If the Source Address is the unspecified address, the node MUST NOT create or
 *                       update the Neighbor Cache entry."
 *
 *                           - (i) "After any updates to the Neighbor Cache, the node sends a Neighbor Advertisement
 *                       response as described in the next section."
 *******************************************************************************************************/
static void NetNDP_RxNeighborSolicitation(const NET_BUF                  *p_buf,
                                          NET_BUF_HDR                    *p_buf_hdr,
                                          const NET_NDP_NEIGHBOR_SOL_HDR *p_ndp_hdr,
                                          RTOS_ERR                       *p_err)
{
  NET_IF_NBR              if_nbr;
  NET_IPv6_ADDR_OBJ       *p_ipv6_addrs;
  NET_IPv6_HDR            *p_ip_hdr;
  NET_CACHE_ADDR_NDP      *p_cache_addr_ndp;
  NET_NDP_OPT_HDR         *p_ndp_opt_hdr;
  NET_NDP_OPT_HW_ADDR_HDR *p_ndp_opt_hw_addr_hdr;
  NET_NDP_OPT_TYPE        opt_type;
  NET_NDP_OPT_LEN         opt_len;
  CPU_INT08U              *p_ndp_opt;
  CPU_INT08U              hop_limit_ip;
  CPU_INT08U              hw_addr[NET_IP_HW_ADDR_LEN];
  CPU_INT08U              hw_addr_len;
  CPU_INT16U              opt_len_tot;
  CPU_INT16U              opt_len_cnt;
  CPU_BOOLEAN             addr_mcast;
  CPU_BOOLEAN             addr_mcast_sol_node;
  CPU_BOOLEAN             addr_unspecified;
  CPU_BOOLEAN             hw_addr_this_host;
  CPU_BOOLEAN             opt_type_src_addr;
  CPU_BOOLEAN             addr_identical;
  CPU_INT32U              timeout_ms;
#ifdef NET_DAD_MODULE_EN
  NET_DAD_OBJ *p_dad_obj;
#endif
  CORE_DECLARE_IRQ_STATE;

  if_nbr = p_buf_hdr->IF_Nbr;

  //                                                               Get IF protocol HW addr and addr size.
  hw_addr_len = sizeof(hw_addr);
  NetIF_AddrHW_GetHandler(if_nbr, hw_addr, &hw_addr_len, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  p_ip_hdr = (NET_IPv6_HDR *)&p_buf->DataPtr[p_buf_hdr->IP_HdrIx];
  hop_limit_ip = p_ip_hdr->HopLim;

  if (hop_limit_ip != NET_IPv6_HDR_HOP_LIM_MAX) {
    NET_CTR_ERR_INC(Net_ErrCtrs.NDP.RxInvalidHopLimit);
    RTOS_ERR_SET(*p_err, RTOS_ERR_RX);                              // See Note #1a.
    goto exit;
  }

  addr_mcast = NetIPv6_IsAddrMcast(&p_ndp_hdr->TargetAddr);
  if (addr_mcast == DEF_YES) {
    NET_CTR_ERR_INC(Net_ErrCtrs.NDP.RxInvalidAddrTarget);
    RTOS_ERR_SET(*p_err, RTOS_ERR_RX);                              // See Note #1e.
    goto exit;
  }

  addr_unspecified = NetIPv6_IsAddrUnspecified(&p_buf_hdr->IPv6_AddrSrc);
  if (addr_unspecified == DEF_YES) {
    addr_mcast_sol_node = NetIPv6_IsAddrMcastSolNode(&p_buf_hdr->IPv6_AddrDest,
                                                     &p_ndp_hdr->TargetAddr);
    if (addr_mcast_sol_node == DEF_NO) {
      NET_CTR_ERR_INC(Net_ErrCtrs.NDP.RxInvalidAddrDest);
      RTOS_ERR_SET(*p_err, RTOS_ERR_RX);                            // See Note #1g.
      goto exit;
    }
  }

  //                                                               Verify if target address is in IF address list.
  p_ipv6_addrs = NetIPv6_GetAddrsHostOnIF(p_buf_hdr->IF_Nbr,
                                          &p_ndp_hdr->TargetAddr);
  if (p_ipv6_addrs != DEF_NULL) {
    if (p_ipv6_addrs->AddrState == NET_IPv6_ADDR_STATE_TENTATIVE) {
      addr_identical = NetIPv6_IsAddrsIdentical(&p_buf_hdr->IPv6_AddrDest, &p_ndp_hdr->TargetAddr);
      if (addr_identical == DEF_YES) {                          // Discard packet if src addr is same as target addr.
        NET_CTR_ERR_INC(Net_ErrCtrs.NDP.RxInvalidAddrDest);
        RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
        goto exit;
      }

      if (addr_unspecified == DEF_NO) {
        NET_CTR_ERR_INC(Net_ErrCtrs.NDP.RxInvalidAddrTarget);
        RTOS_ERR_SET(*p_err, RTOS_ERR_RX);                          // See Note #2b1.
        goto exit;
      } else {
#ifdef  NET_DAD_MODULE_EN
        hw_addr_this_host = Mem_Cmp(&hw_addr[0],
                                    p_buf_hdr->IF_HW_AddrSrcPtr,
                                    p_buf_hdr->IF_HW_AddrLen);
        if (hw_addr_this_host != DEF_YES) {
          p_dad_obj = NetDAD_ObjSrch(&p_ipv6_addrs->AddrHost);
          if (p_dad_obj == DEF_NULL) {
            RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_FOUND);
            goto exit;
          }

          NetDAD_Signal(NET_DAD_SIGNAL_TYPE_ERR, p_dad_obj);

          NET_CTR_ERR_INC(Net_ErrCtrs.NDP.RxAddrDuplicate);
          RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
          goto exit;
        } else {
          NET_CTR_ERR_INC(Net_ErrCtrs.NDP.RxAddrHWThisHost);
          RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
          goto exit;
        }
#else
        NET_CTR_ERR_INC(Net_ErrCtrs.NDP.RxAddrTentative);
        goto exit;
#endif
      }
    }
  }

  opt_len_tot = p_buf_hdr->IP_TotLen - sizeof(NET_NDP_NEIGHBOR_SOL_HDR) + CPU_WORD_SIZE_32;

  p_ndp_opt = (CPU_INT08U *)             &p_ndp_hdr->Opt;
  p_ndp_opt_hdr = (NET_NDP_OPT_HDR *)         p_ndp_opt;
  p_ndp_opt_hw_addr_hdr = (NET_NDP_OPT_HW_ADDR_HDR *) p_ndp_opt;

  if ((addr_unspecified == DEF_YES)                             // Case when the src addr is unspecified and there...
      && (opt_len_tot == 0)) {                                  // ..is no opt field.
    NET_UTIL_IPv6_ADDR_SET_MCAST_ALL_NODES(p_buf_hdr->IPv6_AddrDest);
    p_ndp_opt_hdr->Type = NET_NDP_OPT_TYPE_ADDR_TARGET;         // Set NDP option type for NA to Tx.

    NetICMPv6_TxMsgReply((NET_BUF *)p_buf,
                         p_buf_hdr,
                         (NET_ICMPv6_HDR *)p_ndp_hdr,
                         p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      goto exit;
    }
  } else if ((addr_unspecified == DEF_NO)                       // Case when one or more option fields are present.
             && (opt_len_tot != 0)) {
    opt_type_src_addr = DEF_NO;
    opt_len_cnt = 0u;

    while ((opt_len_cnt < opt_len_tot)
           && (p_ndp_opt != DEF_NULL)
           && (*p_ndp_opt != NET_NDP_OPT_TYPE_NONE)) {
      p_ndp_opt_hdr = (NET_NDP_OPT_HDR *)         p_ndp_opt;
      p_ndp_opt_hw_addr_hdr = (NET_NDP_OPT_HW_ADDR_HDR *) p_ndp_opt;
      opt_type = p_ndp_opt_hdr->Type;
      opt_len = p_ndp_opt_hdr->Len;

      if (opt_len == 0u) {
        NET_CTR_ERR_INC(Net_ErrCtrs.NDP.RxInvalidOptLen);
        RTOS_ERR_SET(*p_err, RTOS_ERR_RX);                          // See Note #1f.
        goto exit;
      }

      if (opt_type == NET_NDP_OPT_TYPE_ADDR_SRC) {              // Only Source Link-Layer Addr Option type is valid.
        opt_type_src_addr = DEF_YES;

        hw_addr_len = (opt_len * DEF_OCTET_NBR_BITS) - (NET_NDP_OPT_DATA_OFFSET);
        if (hw_addr_len != NET_IF_HW_ADDR_LEN_MAX) {
          NET_CTR_ERR_INC(Net_ErrCtrs.NDP.RxInvalidAddrLen);
          RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
          goto exit;
        }

        hw_addr_this_host = Mem_Cmp(&hw_addr[0],
                                    &p_ndp_opt_hw_addr_hdr->Addr,
                                    hw_addr_len);
        if (hw_addr_this_host == DEF_TRUE) {
          NET_CTR_ERR_INC(Net_ErrCtrs.NDP.RxAddrHWThisHost);
          RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
          goto exit;
        }
        //                                                         Search in cache for Address.
        p_cache_addr_ndp = (NET_CACHE_ADDR_NDP *)NetCache_AddrSrch(NET_CACHE_TYPE_NDP,
                                                                   if_nbr,
                                                                   (CPU_INT08U *)&p_buf_hdr->IPv6_AddrSrc,
                                                                   NET_IPv6_ADDR_SIZE);
        if (p_cache_addr_ndp == DEF_NULL) {                     // See Note #2d. If NDP cache not found, ...
          CORE_ENTER_ATOMIC();
          timeout_ms = NetNDP_NeighborCacheTimeout_ms;
          CORE_EXIT_ATOMIC();

          NetNDP_NeighborCacheAddEntry(if_nbr,
                                       (CPU_INT08U *)&p_ndp_opt_hw_addr_hdr->Addr[0],
                                       (CPU_INT08U *)&p_buf_hdr->IPv6_AddrSrc,
                                       DEF_NULL,
                                       timeout_ms,
                                       NetNDP_CacheTimeout,
                                       NET_NDP_CACHE_STATE_STALE,
                                       DEF_NO,
                                       p_err);
          if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
            goto exit;
          }
        } else {                                                // See Note #2e. If NDP cache found, ...
          NetNDP_NeighborCacheUpdateEntry(p_cache_addr_ndp,
                                          &p_ndp_opt_hw_addr_hdr->Addr[0]);
        }

        p_ndp_opt_hdr->Type = NET_NDP_OPT_TYPE_ADDR_TARGET;
        //                                                         See Note #2i.
        NetICMPv6_TxMsgReply((NET_BUF *)p_buf,
                             p_buf_hdr,
                             (NET_ICMPv6_HDR *)p_ndp_hdr,
                             p_err);
        if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
          goto exit;
        }
      }

      p_ndp_opt += (opt_len * DEF_OCTET_NBR_BITS);
      opt_len_cnt += (opt_len * DEF_OCTET_NBR_BITS);
    }

    if (opt_type_src_addr == DEF_NO) {                          // Case when no Source Link-Layer Address option ...
      NET_CTR_ERR_INC(Net_ErrCtrs.NDP.RxInvalidOptType);
      RTOS_ERR_SET(*p_err, RTOS_ERR_RX);                            // ... is included in received NS.
      goto exit;
    }
  } else if (addr_unspecified == DEF_NO                         // RFC#4861 s7.2.4 p.64
             && (opt_len_tot == 0)) {                           // Case when no option is included in the NS.
    p_ndp_opt_hdr->Type = NET_NDP_OPT_TYPE_ADDR_TARGET;

    NetICMPv6_TxMsgReply((NET_BUF *)p_buf,
                         p_buf_hdr,
                         (NET_ICMPv6_HDR *)p_ndp_hdr,
                         p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      goto exit;
    }
  } else {
    NET_CTR_ERR_INC(Net_ErrCtrs.NDP.RxInvalidAddrSrc);
    RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
    goto exit;
  }

exit:
  return;
}

/****************************************************************************************************//**
 *                                       NetNDP_RxNeighborAdvertisement()
 *
 * @brief    Receive Neighbor Advertisement message.
 *
 * @param    p_buf       Pointer to network buffer that received NDP packet.
 *
 * @param    p_buf_hdr   Pointer to network buffer header.
 *
 * @param    p_ndp_hdr   Pointer to received packet's NDP header.
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function.
 *
 * @note     (1) RFC #4861, Section 7.1.2 states that "A node MUST silently discard any received
 *                       Neighbor Advertisement messages that does not satisfy all of the following validity
 *                       checks":
 *
 *                           - (a) The IP Hop Limit field has a value of 255.
 *
 *                           - (b) ICMP checksum     is valid.
 *
 *                           - (d) ICMP code         is zero.
 *
 *                           - (e) ICMP length (derived from the IP length) is 24 or more octets.
 *
 *                           - (f) Target address is NOT a multicast address.
 *
 *                           - (g) If the IP destination address is a multicast address, the solicited flag is zero.
 *
 *                           - (h) All included options have a length that is greater than zero.
 *
 * @note     (2) RFC #4862, Section 5.4.4 states that "On receipt of a valid Neighbor Advertisement
 *                       message on an interface, node behavior depends on whether the target address is
 *                       tentative or matches a unicast or anycast address assigned to the interface: "
 *
 *                           - (a) "If the target address is tentative, the tentative address is not unique."
 *
 *                           - (b) "If the target address matches a unicast address assigned to the receiving
 *                       interface, it would possibly indicate that the address is a duplicate but it
 *                       has not been detected by the Duplicate Address Detection procedure (recall that
 *                       Duplicate Address Detection is not completely reliable).  How to handle such a
 *                       case is beyond the scope of this document."
 *
 *                           - (c) "Otherwise, the advertisement is processed as described in [RFC4861]."
 *
 * @note     (3) RFC #4861, Section 7.2.5 details the receipt of a Neighbor Advertisement Message :
 *
 *                           - (a) "When a valid Neighbor Advertisement is received (either solicited or
 *                       unsolicited), the Neighbor Cache is searched for the target's entry. If no
 *                       entry exists, the advertisement SHOULD be silently discarded."
 *
 *                           - (b) "If the target's Neighbor Cache entry is in the INCOMPLETE state when the
 *                       advertisement is received, one of two things happens :
 *
 *                           - (1) "If the link layer has addresses and no Target Link-Layer Address option is
 *                       included, the receiving node SHOULD silently discard the received."
 *
 *                           - (2) "Otherwise, the receiving node performs the following steps:"
 *
 *                           - (A) "It records the link-layer address in the Neighbor Cache entry"
 *
 *                           - (B) "If the advertisement's Solicited flag is set, the state of the entry is
 *                       set to REACHABLE; otherwise, it is set to STALE"
 *
 *                           - (C) "It sets the IsRouter flag in the cache entry based on the Router flag
 *                       in the received advertisement"
 *
 *                           - (D) "It sends any packets queued for the neighbor awaiting address
 *                       resolution."
 *
 *                           - (c) "If the target's Neighbor Cache entry is in any state other than INCOMPLETE when
 *                       the advertisement is received, the following actions take place:"
 *
 *                           - (1) "If the Override flag is clear and the supplied link-layer address differs
 *                       from that in the cache, then one of two actions takes place:"
 *                           - (A) "If the state of the entry is REACHABLE, set it to STALE, but do not
 *                       update the entry in any other way."
 *
 *                           - (B) "Otherwise, the received advertisement should be ignored and MUST NOT
 *                       update the cache"
 *
 *                           - (2) "If the Override flag is set, or the supplied link-layer address is the same
 *                       as that in the cache, or no Target Link-Layer Address option was supplied,
 *                       the received advertisement MUST update the Neighbor Cache entry as follows:"
 *
 *                           - (A) "The link-layer address in the Target Link-Layer Address option MUST be
 *                       inserted in the cache (if one is supplied and differs from the already
 *                       recorded address)"
 *
 *                           - (B) "If the Solicited flag is set, the state of the entry MUST be set to
 *                       REACHABLE.  If the Solicited flag is zero and the link-layer address
 *                       was updated with a different address, the state MUST be set to STALE.
 *                       Otherwise, the entry's state remains unchanged."
 *
 *                           - (C) "The IsRouter flag in the cache entry MUST be set based on the Router
 *                       flag in the received advertisement.  In those cases where the IsRouter
 *                       flag changes from TRUE to FALSE as a result of this update, the node
 *                       MUST remove that router from the Default Router List and update the
 *                       Destination Cache entries for all destinations using that neighbor as a
 *                       router as specified in Section 7.3.3.  This is needed to detect when a
 *                       node that is used as a router stops forwarding packets due to being
 *                       configured as a host."
 *
 *                           - (d) "If none of the above apply, the advertisement prompts future Neighbor
 *                       Unreachability Detection (if it is not already in progress) by changing the
 *                       state in the cache entry."
 *******************************************************************************************************/
static void NetNDP_RxNeighborAdvertisement(const NET_BUF                  *p_buf,
                                           NET_BUF_HDR                    *p_buf_hdr,
                                           const NET_NDP_NEIGHBOR_ADV_HDR *p_ndp_hdr,
                                           RTOS_ERR                       *p_err)
{
  NET_CACHE_ADDR_NDP      *p_cache_addr_ndp;
  NET_NDP_NEIGHBOR_CACHE  *p_cache_ndp;
  NET_NDP_ROUTER          *p_router;
  NET_NDP_OPT_HDR         *p_ndp_opt_hdr;
  NET_NDP_OPT_HW_ADDR_HDR *p_ndp_opt_hw_addr_hdr = DEF_NULL;
  NET_IPv6_HDR            *p_ip_hdr;
  NET_IPv6_ADDR_OBJ       *p_ipv6_addrs;
  NET_BUF                 *p_buf_head;
  CPU_INT08U              *p_ndp_opt;
  NET_NDP_OPT_TYPE        opt_type;
  NET_NDP_OPT_LEN         opt_len;
  CPU_INT32U              timeout_ms;
  NET_IF_NBR              if_nbr;
  CPU_FNCT_PTR            tmr_fnct;
  CPU_INT08U              hw_addr_len;
  CPU_INT08U              cache_state;
  CPU_INT08U              hop_limit_ip;
  CPU_INT16U              opt_len_tot;
  CPU_INT16U              opt_len_cnt;
  CPU_INT32U              flags;
  CPU_BOOLEAN             is_solicited;
  CPU_BOOLEAN             is_override;
  CPU_BOOLEAN             is_router;
  CPU_BOOLEAN             addr_mcast;
  CPU_BOOLEAN             same_hw_addr;
  CPU_BOOLEAN             opt_type_target_addr;
  CPU_BOOLEAN             addr_identical;
#ifdef NET_DAD_MODULE_EN
  NET_DAD_OBJ *p_dad_obj;
#endif
  CORE_DECLARE_IRQ_STATE;

  if_nbr = p_buf_hdr->IF_Nbr;

  NET_UTIL_VAL_COPY_GET_NET_32(&flags, &p_ndp_hdr->Flags);

  is_override = DEF_BIT_IS_SET(flags, NET_NDP_HDR_FLAG_OVRD);
  is_solicited = DEF_BIT_IS_SET(flags, NET_NDP_HDR_FLAG_SOL);
  is_router = DEF_BIT_IS_SET(flags, NET_NDP_HDR_FLAG_ROUTER);

  //                                                               ----------------- RX NA VALIDATION -----------------
  p_ip_hdr = (NET_IPv6_HDR *)&p_buf->DataPtr[p_buf_hdr->IP_HdrIx];
  hop_limit_ip = p_ip_hdr->HopLim;
  if (hop_limit_ip != NET_IPv6_HDR_HOP_LIM_MAX) {               // See Note #1a.
    NET_CTR_ERR_INC(Net_ErrCtrs.NDP.RxInvalidHopLimit);
    RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
    goto exit;
  }

  addr_mcast = NetIPv6_IsAddrMcast(&p_ndp_hdr->TargetAddr);
  if (addr_mcast == DEF_YES) {                                  // See Note #1f.
    NET_CTR_ERR_INC(Net_ErrCtrs.NDP.RxInvalidAddrTarget);
    RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
    goto exit;
  }

  addr_mcast = NetIPv6_IsAddrMcast(&p_buf_hdr->IPv6_AddrDest);
  if (addr_mcast == DEF_YES) {                                  // See Note #1g.
    if (is_solicited == DEF_YES) {
      NET_CTR_ERR_INC(Net_ErrCtrs.NDP.RxInvalidAddrDest);
      RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
      goto exit;
    }
  }

  //                                                               --------- SEARCH IN IF IPv6 CFGD ADDR LIST --------
  p_ipv6_addrs = NetIPv6_GetAddrsHostOnIF(p_buf_hdr->IF_Nbr,
                                          &p_ndp_hdr->TargetAddr);

  if (p_ipv6_addrs != DEF_NULL) {
    switch (p_ipv6_addrs->AddrState) {
      case NET_IPv6_ADDR_STATE_TENTATIVE:
        addr_identical = NetIPv6_IsAddrsIdentical(&p_buf_hdr->IPv6_AddrDest, &p_ndp_hdr->TargetAddr);
        if (addr_identical == DEF_YES) {                        // Discard packet if src addr is same as target addr.
          NET_CTR_ERR_INC(Net_ErrCtrs.NDP.RxInvalidAddrDest);
          RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
          goto exit;
        }
#ifdef  NET_DAD_MODULE_EN
        p_dad_obj = NetDAD_ObjSrch(&p_ipv6_addrs->AddrHost);
        if (p_dad_obj == DEF_NULL) {
          RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_FOUND);
          goto exit;
        }

        NetDAD_Signal(NET_DAD_SIGNAL_TYPE_ERR, p_dad_obj);          // See Note #2a.
#endif
        NET_CTR_ERR_INC(Net_ErrCtrs.NDP.RxAddrDuplicate);
        RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
        goto exit;

      case NET_IPv6_ADDR_STATE_PREFERRED:                       // See Note #2b.
      case NET_IPv6_ADDR_STATE_DEPRECATED:
#ifdef  NET_DAD_MODULE_EN
        NET_CTR_ERR_INC(Net_ErrCtrs.NDP.RxNeighborAdvAddrDuplicateCtr);
#endif
        NET_CTR_ERR_INC(Net_ErrCtrs.NDP.RxAddrDuplicate);
        RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
        goto exit;

      case NET_IPv6_ADDR_STATE_NONE:
      case NET_IPv6_ADDR_STATE_DUPLICATED:
      default:
        break;
    }
  }

  //                                                               --- SEARCH IN NEIGHBOR CACHE FOR IPv6 TARGET ADDR --
  p_cache_addr_ndp = (NET_CACHE_ADDR_NDP *)NetCache_AddrSrch(NET_CACHE_TYPE_NDP,
                                                             if_nbr,
                                                             (CPU_INT08U *)&p_ndp_hdr->TargetAddr,
                                                             NET_IPv6_ADDR_SIZE);

  if (p_cache_addr_ndp == DEF_NULL) {                           // See Note #3a.
    RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_FOUND);
    goto exit;
  }

  p_cache_ndp = (NET_NDP_NEIGHBOR_CACHE *) p_cache_addr_ndp->ParentPtr;
  cache_state = p_cache_ndp->State;

  //                                                               ------------- SCAN NA FOR VALID OPTIONS ------------
  opt_len_tot = p_buf_hdr->IP_TotLen - sizeof(NET_NDP_NEIGHBOR_ADV_HDR) + CPU_WORD_SIZE_32;
  p_ndp_opt = (CPU_INT08U *)&p_ndp_hdr->Opt;
  opt_type_target_addr = DEF_NO;
  opt_len_cnt = 0u;

  while ((p_ndp_opt != DEF_NULL)
         && (opt_len_cnt < opt_len_tot)) {
    p_ndp_opt_hdr = (NET_NDP_OPT_HDR *)p_ndp_opt;
    opt_type = p_ndp_opt_hdr->Type;
    opt_len = p_ndp_opt_hdr->Len;

    if (opt_len == 0u) {
      NET_CTR_ERR_INC(Net_ErrCtrs.NDP.RxInvalidOptLen);
      RTOS_ERR_SET(*p_err, RTOS_ERR_RX);                            // See Note #1f.
      goto exit;
    }

    switch (opt_type) {
      case NET_NDP_OPT_TYPE_ADDR_TARGET:
        opt_type_target_addr = DEF_YES;
        p_ndp_opt_hw_addr_hdr = (NET_NDP_OPT_HW_ADDR_HDR *) p_ndp_opt;

        hw_addr_len = (opt_len * DEF_OCTET_NBR_BITS) - (NET_NDP_OPT_DATA_OFFSET);

        if (hw_addr_len != NET_IF_HW_ADDR_LEN_MAX) {
          NET_CTR_ERR_INC(Net_ErrCtrs.NDP.RxInvalidAddrLen);
          RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
          goto exit;
        }
        break;

      case NET_NDP_OPT_TYPE_NONE:
      case NET_NDP_OPT_TYPE_ADDR_SRC:
      case NET_NDP_OPT_TYPE_PREFIX_INFO:
      case NET_NDP_OPT_TYPE_REDIRECT:
      case NET_NDP_OPT_TYPE_MTU:
      default:
        break;
    }

    p_ndp_opt += (opt_len * DEF_OCTET_NBR_BITS);
    opt_len_cnt += (opt_len * DEF_OCTET_NBR_BITS);
  }

  //                                                               ------------------- RX NA HANDLING ------------------
  if (cache_state == NET_NDP_CACHE_STATE_INCOMPLETE) {
    if (opt_type_target_addr == DEF_YES) {
      Mem_Copy(&p_cache_addr_ndp->AddrHW[0],                    // See Note #3b2A.
               &p_ndp_opt_hw_addr_hdr->Addr[0],
               NET_IF_HW_ADDR_LEN_MAX);

      p_cache_addr_ndp->AddrHW_Valid = DEF_YES;

      //                                                           See Note #3b2B.
      if (is_solicited == DEF_YES) {
        CORE_ENTER_ATOMIC();
        timeout_ms = NetNDP_DataPtr->ReachableTimeout_ms;
        CORE_EXIT_ATOMIC();
        tmr_fnct = NetNDP_ReachableTimeout;
        p_cache_ndp->State = NET_NDP_CACHE_STATE_REACHABLE;
      } else {
        CORE_ENTER_ATOMIC();
        timeout_ms = NetNDP_NeighborCacheTimeout_ms;
        CORE_EXIT_ATOMIC();
        tmr_fnct = NetNDP_CacheTimeout;
        p_cache_ndp->State = NET_NDP_CACHE_STATE_STALE;
      }

      //                                                           Reset cache tmr.
      NetTmr_Set(p_cache_ndp->TmrPtr,
                 tmr_fnct,
                 timeout_ms);

      //                                                           See Note #3b2C.
      if (is_router == DEF_YES) {
        DEF_BIT_SET(p_cache_ndp->Flags, NET_NDP_CACHE_FLAG_ISROUTER);
      }

      //                                                           Re-initialize nbr of Solicitations sent.
      p_cache_ndp->ReqAttemptsCtr = 0;

      //                                                           See Note #3b2D.
      p_buf_head = p_cache_addr_ndp->TxQ_Head;
      p_cache_addr_ndp->TxQ_Head = DEF_NULL;
      p_cache_addr_ndp->TxQ_Tail = DEF_NULL;
      p_cache_addr_ndp->TxQ_Nbr = 0;

      NetCache_TxPktHandler(NET_PROTOCOL_TYPE_NDP,
                            p_buf_head,
                            &p_ndp_opt_hw_addr_hdr->Addr[0]);
    } else {                                                    // See Note #3b1.
      NET_CTR_ERR_INC(Net_ErrCtrs.NDP.RxInvalidOptType);
      RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
      goto exit;
    }
  } else {                                                      // Neighbor cache state other than INCOMPLETE.
    if (p_ndp_opt_hw_addr_hdr != DEF_NULL) {
      same_hw_addr = Mem_Cmp(&p_cache_addr_ndp->AddrHW,
                             &p_ndp_opt_hw_addr_hdr->Addr,
                             NET_IF_HW_ADDR_LEN_MAX);
    } else {
      same_hw_addr = DEF_NO;
    }

    if ((is_override == DEF_NO)
        && (same_hw_addr == DEF_NO)) {                          // See Note #3c1.
      if (p_cache_ndp->State == NET_NDP_CACHE_STATE_REACHABLE) {        // See Note #3c1A & #3c1B.
        p_cache_ndp->State = NET_NDP_CACHE_STATE_STALE;
        CORE_ENTER_ATOMIC();
        timeout_ms = NetNDP_NeighborCacheTimeout_ms;
        CORE_EXIT_ATOMIC();
        NetTmr_Set(p_cache_ndp->TmrPtr,
                   NetNDP_CacheTimeout,
                   timeout_ms);
      }
    } else if ((is_override == DEF_YES)                         // See Note #3c2.
               || (same_hw_addr == DEF_YES)
               || (opt_type_target_addr == DEF_NO)) {
      if (opt_type_target_addr == DEF_YES                       // See Note #3c2A.
          && same_hw_addr == DEF_NO) {
        Mem_Copy(&p_cache_addr_ndp->AddrHW,
                 &p_ndp_opt_hw_addr_hdr->Addr,
                 NET_IF_HW_ADDR_LEN_MAX);
      }

      if (is_solicited == DEF_YES) {                            // See Note #3c2B.
        p_cache_ndp->State = NET_NDP_CACHE_STATE_REACHABLE;
        CORE_ENTER_ATOMIC();
        timeout_ms = NetNDP_DataPtr->ReachableTimeout_ms;
        CORE_EXIT_ATOMIC();
        NetTmr_Set(p_cache_ndp->TmrPtr,
                   NetNDP_ReachableTimeout,
                   timeout_ms);
      } else {
        if (same_hw_addr == DEF_NO) {
          p_cache_ndp->State = NET_NDP_CACHE_STATE_STALE;
          CORE_ENTER_ATOMIC();
          timeout_ms = NetNDP_NeighborCacheTimeout_ms;
          CORE_EXIT_ATOMIC();
          NetTmr_Set(p_cache_ndp->TmrPtr,
                     NetNDP_CacheTimeout,
                     timeout_ms);
        }
      }
      //                                                           See Note #3c2C.
      if (is_router == DEF_YES) {
        DEF_BIT_SET(p_cache_ndp->Flags, NET_NDP_CACHE_FLAG_ISROUTER);
      } else {
        DEF_BIT_CLR(p_cache_ndp->Flags, (CPU_INT16U)NET_NDP_CACHE_FLAG_ISROUTER);

        p_router = NetNDP_RouterSrch(if_nbr,
                                     (NET_IPv6_ADDR *)&p_cache_addr_ndp->AddrProtocol[0],
                                     p_err);
        if (p_router != DEF_NULL) {
          NetNDP_RemoveAddrDestCache(p_cache_addr_ndp->IF_Nbr, &p_cache_addr_ndp->AddrProtocol[0]);
          NetNDP_RouterRemove(p_router, DEF_YES);
          (void)NetNDP_UpdateDefaultRouter(p_cache_addr_ndp->IF_Nbr);
        }
      }
    }
  }

exit:
  return;
}

/****************************************************************************************************//**
 *                                           NetNDP_RxRedirect()
 *
 * @brief    Receive Redirect message.
 *
 * @param    p_buf       Pointer to network buffer that received ICMP packet.
 *
 * @param    p_buf_hdr   Pointer to network buffer header.
 *
 * @param    p_ndp_hdr   Pointer to received packet's NDP header.
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function.
 *
 * @note     (1) RFC #4861, Section 8.1 states that "A host MUST silently discard any received
 *                       Redirect messages that does not satisfy all of the following validity
 *                       checks":
 *
 *                           - (a) IP Source Address is a link-local address.
 *
 *                           - (b) The IP Hop Limit field has a value of 255.
 *
 *                           - (c) ICMP checksum     is valid.
 *
 *                           - (d) ICMP code         is zero.
 *
 *                           - (e) ICMP length (derived from the IP length) is 40 or more octets.
 *
 *                           - (f) The IP source address of the Redirect is the same as the current first-hop
 *                       router for the specified ICMP Destination Address.
 *
 *                           - (g) The ICMP Destination Address field in the redirect message does not contain
 *                       a multicast address.
 *
 *                           - (h) The ICMP Target Address is either a link-local address (when redirected to a
 *                       router) or the same as the ICMP Destination Address (when redirected to the
 *                       on-link-destination.
 *
 *                           - (i) "All included options have a length that is greater than zero."
 *
 * @note     (2) If address target is the same as the destination address, the destination is on-link.
 *                       If not, the target address contain the better first-hop router.
 *
 * @note     (3) The following information is not used by the current implementation of NDP:
 *
 *                           - (a) Redirected header option. It should contain as much as possible of the IP
 *                       packet that triggered the sending of the Redirect without making the redirect
 *                       packet exceed the minimum MTU.
 *******************************************************************************************************/
static void NetNDP_RxRedirect(const NET_BUF              *p_buf,
                              NET_BUF_HDR                *p_buf_hdr,
                              const NET_NDP_REDIRECT_HDR *p_ndp_hdr,
                              RTOS_ERR                   *p_err)
{
  NET_CACHE_ADDR_NDP       *p_cache_addr_target;
  NET_NDP_NEIGHBOR_CACHE   *p_cache_target;
  NET_NDP_DEST_CACHE       *p_dest_cache;
  NET_NDP_OPT_TYPE         opt_type;
  NET_NDP_OPT_LEN          opt_len;
  CPU_INT16U               opt_len_tot;
  CPU_INT16U               opt_len_cnt;
  CPU_INT08U               *p_ndp_opt;
  NET_NDP_OPT_HDR          *p_ndp_opt_hdr;
  NET_NDP_OPT_HW_ADDR_HDR  *p_ndp_opt_hw_addr_hdr;
  NET_NDP_OPT_REDIRECT_HDR *p_ndp_opt_redirect_hdr;
  CPU_INT08U               hw_addr_len;
  CPU_INT08U               hop_limit_ip;
  NET_IPv6_HDR             *p_ip_hdr;
  NET_IF_NBR               if_nbr;
  CPU_INT32U               timeout_ms;
  CPU_BOOLEAN              addr_is_link_local;
  CPU_BOOLEAN              addr_is_mcast;
  CPU_BOOLEAN              mem_same;
  CPU_BOOLEAN              is_router;
  CORE_DECLARE_IRQ_STATE;

  if_nbr = p_buf_hdr->IF_Nbr;
  //                                                               -------- VALIDATION OF THE REDIRECT MESSAGE --------
  addr_is_link_local = NetIPv6_IsAddrLinkLocal(&p_buf_hdr->IPv6_AddrSrc);
  if (addr_is_link_local == DEF_NO) {
    NET_CTR_ERR_INC(Net_ErrCtrs.NDP.RxInvalidAddrSrc);
    RTOS_ERR_SET(*p_err, RTOS_ERR_RX);                              // See Note #1a.
    goto exit;
  }

  p_ip_hdr = (NET_IPv6_HDR *)&p_buf->DataPtr[p_buf_hdr->IP_HdrIx];
  hop_limit_ip = p_ip_hdr->HopLim;
  if (hop_limit_ip != NET_IPv6_HDR_HOP_LIM_MAX) {
    NET_CTR_ERR_INC(Net_ErrCtrs.NDP.RxInvalidHopLimit);
    RTOS_ERR_SET(*p_err, RTOS_ERR_RX);                              // See Note #1b.
    goto exit;
  }

  addr_is_mcast = NetIPv6_IsAddrMcast(&p_ndp_hdr->AddrDest);
  if (addr_is_mcast == DEF_YES) {
    NET_CTR_ERR_INC(Net_ErrCtrs.NDP.RxInvalidAddrDest);
    RTOS_ERR_SET(*p_err, RTOS_ERR_RX);                              // See Note #1g.
    goto exit;
  }

  //                                                               Search in cache for Destination Address.
  p_dest_cache = NetNDP_DestCacheSrch(if_nbr, &p_ndp_hdr->AddrDest);
  if (p_dest_cache == DEF_NULL) {
    NET_CTR_ERR_INC(Net_ErrCtrs.NDP.RxInvalidAddrDest);
    RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
    goto exit;
  }

  mem_same = Mem_Cmp(&p_buf_hdr->IPv6_AddrSrc.Addr[0],
                     &p_dest_cache->AddrNextHop.Addr[0],
                     NET_IPv6_ADDR_SIZE);
  if (mem_same == DEF_NO) {
    NET_CTR_ERR_INC(Net_ErrCtrs.NDP.RxInvalidAddrSrc);
    RTOS_ERR_SET(*p_err, RTOS_ERR_RX);                              // See Note #1f.
    goto exit;
  }

  addr_is_link_local = NetIPv6_IsAddrLinkLocal(&p_ndp_hdr->AddrTarget);

  mem_same = Mem_Cmp(&p_ndp_hdr->AddrTarget,
                     &p_ndp_hdr->AddrDest,
                     NET_IPv6_ADDR_SIZE);

  is_router = !mem_same;

  if ((addr_is_link_local != DEF_YES)
      && (mem_same != DEF_YES)) {
    NET_CTR_ERR_INC(Net_ErrCtrs.NDP.RxInvalidAddrTarget);
    RTOS_ERR_SET(*p_err, RTOS_ERR_RX);                              // See Note #1h.
    goto exit;
  }

  //                                                               ----------- VALIDATE ADN PROCESS OPTIONS -----------
  opt_len_tot = p_buf_hdr->IP_TotLen - sizeof(NET_NDP_REDIRECT_HDR) + sizeof(CPU_INT32U);
  p_ndp_opt = (CPU_INT08U *)&p_ndp_hdr->Opt;

  opt_len_cnt = 0u;
  while ((p_ndp_opt != DEF_NULL)
         && (*p_ndp_opt != NET_NDP_OPT_TYPE_NONE)
         && (opt_len_cnt < opt_len_tot)) {
    p_ndp_opt_hdr = (NET_NDP_OPT_HDR *)p_ndp_opt;
    opt_type = p_ndp_opt_hdr->Type;
    opt_len = p_ndp_opt_hdr->Len;

    if (opt_len == 0u) {
      NET_CTR_ERR_INC(Net_ErrCtrs.NDP.RxInvalidOptLen);
      RTOS_ERR_SET(*p_err, RTOS_ERR_RX);                            // See Note #1i.
      goto exit;
    }

    switch (opt_type) {
      case NET_NDP_OPT_TYPE_ADDR_TARGET:
        p_ndp_opt_hw_addr_hdr = (NET_NDP_OPT_HW_ADDR_HDR *) p_ndp_opt;
        hw_addr_len = (opt_len * DEF_OCTET_NBR_BITS) - (NET_NDP_OPT_DATA_OFFSET);
        if (hw_addr_len != NET_IF_HW_ADDR_LEN_MAX) {
          NET_CTR_ERR_INC(Net_ErrCtrs.NDP.RxInvalidAddrLen);
          RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
          goto exit;
        }

        //                                                         Search in cache for Target Address.
        p_cache_addr_target = (NET_CACHE_ADDR_NDP *)NetCache_AddrSrch(NET_CACHE_TYPE_NDP,
                                                                      if_nbr,
                                                                      (CPU_INT08U *)&p_ndp_hdr->AddrTarget.Addr[0],
                                                                      NET_IPv6_ADDR_SIZE);
        if (p_cache_addr_target == DEF_NULL) {
          CORE_ENTER_ATOMIC();
          timeout_ms = NetNDP_NeighborCacheTimeout_ms;
          CORE_EXIT_ATOMIC();

          NetNDP_NeighborCacheAddEntry(if_nbr,
                                       (CPU_INT08U *)&p_ndp_opt_hw_addr_hdr->Addr[0],
                                       (CPU_INT08U *)&p_ndp_hdr->AddrTarget,
                                       DEF_NULL,
                                       timeout_ms,
                                       NetNDP_CacheTimeout,
                                       NET_NDP_CACHE_STATE_STALE,
                                       is_router,
                                       p_err);
        } else {
          NetNDP_NeighborCacheUpdateEntry(p_cache_addr_target,
                                          &p_ndp_opt_hw_addr_hdr->Addr[0]);

          p_cache_target = (NET_NDP_NEIGHBOR_CACHE *)p_cache_addr_target->ParentPtr;
          if (is_router == DEF_YES) {
            DEF_BIT_SET(p_cache_target->Flags, NET_NDP_CACHE_FLAG_ISROUTER);
          }
        }
        break;

      case NET_NDP_OPT_TYPE_REDIRECT:
#if 1                                                           // Prevent compiler warning.
        PP_UNUSED_PARAM(p_ndp_opt_redirect_hdr);                // See Note #3a
#else
        p_ndp_opt_redirect_hdr = (NET_NDP_OPT_REDIRECT_HDR *) p_ndp_opt;
#endif

        break;

      case NET_NDP_OPT_TYPE_ADDR_SRC:
      case NET_NDP_OPT_TYPE_PREFIX_INFO:
      case NET_NDP_OPT_TYPE_MTU:
      default:
        NET_CTR_ERR_INC(Net_ErrCtrs.NDP.RxInvalidOptType);
        RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
        break;
    }

    p_ndp_opt += (opt_len * DEF_OCTET_NBR_BITS);
    opt_len_cnt += (opt_len * DEF_OCTET_NBR_BITS);
  }

  //                                                               ------------- UPDATE DESTINATION CACHE -------------
  p_dest_cache->AddrNextHop = p_ndp_hdr->AddrTarget;

  if (mem_same == DEF_YES) {
    p_dest_cache->OnLink = DEF_YES;
  }

  NetNDP_UpdateDestCache(if_nbr,
                         &p_ndp_hdr->AddrDest.Addr[0],
                         &p_ndp_hdr->AddrTarget.Addr[0]);

exit:
  return;
}

/****************************************************************************************************//**
 *                                           NetNDP_RxPrefixUpdate()
 *
 * @brief    (1) Add or Update an NDP prefix entry in the prefix list based on the received NDP prefix.
 *
 *           - (a) Search NDP prefix list
 *           - (b) Add or Update NDP prefix
 *           - (c) If Autonomous Flag is set, configure new addr with received prefix on Interface
 *
 * @param    if_nbr          Interface number the packet was received from.
 *
 * @param    p_addr_prefix   Pointer to received NDP prefix.
 *
 * @param    p_err           Pointer to variable that will receive the return error code from this function.
 *
 * @note     (2) RFC #4861, Section 6.3.4 (Processing Received Router Advertisement) states that "For
 *                           each Prefix Information option with the on-link flag set, a host does the following:"
 *
 *                               - (a) "If the prefix is the link-local prefix, silently ignore the Prefix Information option."
 *
 *                               - (b) "If the prefix is not already present in the Prefix List, and the Prefix Information
 *                           option's Valid Lifetime field is non-zero, create a new entry for the prefix and
 *                           initialize its invalidation timer to the Valid Lifetime value in the Prefix
 *                           Information option."
 *
 *                               - (c) "If the prefix is already present in the host's Prefix List as the result of a
 *                           previously received advertisement, reset its invalidation timer to the Valid Lifetime
 *                           value in the Prefix Information option.  If the new Lifetime value is zero, time-out
 *                           the prefix immediately (see Section 6.3.5)."
 *
 *                               - (d) "If the Prefix Information option's Valid Lifetime field is zero, and the prefix is
 *                           not present in the host's Prefix List, silently ignore the option."
 *
 * @note     (3) RFC #4862, Section 5.5.3 states that "For each Prefix-Information option in the
 *                           Router Advertisement:
 *
 *                               - (a) If the Autonomous flag is not set,                            silently ignore the
 *                           Prefix Information option.
 *
 *                               - (b) If the prefix is the link-local prefix,                       silently ignore the
 *                           Prefix Information option.
 *
 *                               - (c) If the preferred lifetime is greater than the valid lifetime, silently ignore the
 *                           Prefix Information option.
 *
 *                               - (d) If the prefix advertised is not equal to the prefix of an address configured
 *                           by stateless autoconfiguration already in the list of addresses associated
 *                           with the interface (where "equal" means the two prefix lengths are the same
 *                           and the first prefix-length bits of the prefixes are identical), and if the
 *                           Valid Lifetime is not 0, form an address (and add it to the list) by combining
 *                           the advertised prefix with an interface identifier of the link as follows:
 *
 *                           |            128 - N bits               |       N bits           |
 *                           +---------------------------------------+------------------------+
 *                           |            link prefix                |  interface identifier  |
 *                           +----------------------------------------------------------------+
 *******************************************************************************************************/
static void NetNDP_RxPrefixUpdate(NET_IF_NBR    if_nbr,
                                  NET_IPv6_ADDR *p_addr_prefix,
                                  CPU_INT08U    prefix_len,
                                  CPU_BOOLEAN   on_link,
                                  CPU_BOOLEAN   addr_cfg_auto,
                                  CPU_INT32U    lifetime_valid,
                                  CPU_INT32U    lifetime_preferred,
                                  RTOS_ERR      *p_err)
{
  NET_IF                   *p_if;
  NET_IPv6_ADDR_OBJ        *p_addrs;
  NET_IPv6_ADDR            ipv6_id;
  CPU_BOOLEAN              updated;
  CPU_BOOLEAN              dad_en;
  CPU_INT08U               id_len;
  CPU_INT08U               total_addr_len;
  NET_IPv6_ADDR_CFG_TYPE   addr_cfg_type;
  NET_IPv6_ADDR_CFG_STATUS cfg_status;
#ifdef  NET_IPv6_ADDR_AUTO_CFG_MODULE_EN
  NET_IPv6_AUTO_CFG_OBJ   *p_auto_obj;
  NET_IPv6_AUTO_CFG_STATE auto_state = NET_IPv6_AUTO_CFG_STATE_NONE;
  CORE_DECLARE_IRQ_STATE;
#endif

  p_if = NetIF_Get(if_nbr, p_err);                              // retrieve Interface object.
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  PP_UNUSED_PARAM(p_if);

  //                                                               --------- ADD/UPDATE PREFIX TO PREFIX LIST ---------
  NetNDP_RxPrefixHandler(if_nbr,
                         p_addr_prefix,
                         prefix_len,
                         lifetime_valid,
                         on_link,
                         addr_cfg_auto,
                         p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  //                                                               ---------- CREATE & ADD ADDR WITH PREFIX -----------
  if (addr_cfg_auto == DEF_YES) {                               // When Autonomous Flag is set.
    if (lifetime_valid < lifetime_preferred) {
      NET_CTR_ERR_INC(Net_ErrCtrs.NDP.RxInvalidPrefix);
      RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
      goto exit;
    }

#ifdef  NET_IPv6_ADDR_AUTO_CFG_MODULE_EN

    p_auto_obj = p_if->IP_Obj->IPv6_AutoCfgObjPtr;
    if (p_auto_obj == DEF_NULL) {
      addr_cfg_type = NET_IPv6_ADDR_CFG_TYPE_RX_PREFIX_INFO;
      dad_en = DEF_YES;
    } else {
      //                                                           Post signal for Rx RA msg with prefix option.
      NetTmr_Free(p_auto_obj->RouterSolRetryTmr);
      p_auto_obj->RouterSolRetryTmr = DEF_NULL;

      //                                                           ------------------ SET DAD TYPE --------------------
      CORE_ENTER_ATOMIC();
      auto_state = p_auto_obj->State;
      CORE_EXIT_ATOMIC();
      switch (auto_state) {
        case NET_IPv6_AUTO_CFG_STATE_STARTED_GLOBAL:
          addr_cfg_type = NET_IPv6_ADDR_CFG_TYPE_AUTO_CFG_NO_BLOCKING;
          dad_en = p_auto_obj->DAD_En;
          break;

        case NET_IPv6_AUTO_CFG_STATE_STOPPED:
          goto exit_fail_stop_auto;

        default:
          addr_cfg_type = NET_IPv6_ADDR_CFG_TYPE_RX_PREFIX_INFO;
          dad_en = DEF_YES;
          break;
      }
    }

#else
    addr_cfg_type = NET_IPv6_ADDR_CFG_TYPE_RX_PREFIX_INFO;
    dad_en = DEF_YES;
#endif

    //                                                             Update addr(s) with same prefix already cfg on IF.
    updated = NetNDP_RxPrefixAddrsUpdate(if_nbr,
                                         p_addr_prefix,
                                         prefix_len,
                                         lifetime_valid,
                                         lifetime_preferred);

    if ((updated == DEF_NO)
        && (lifetime_valid > 0)) {
      id_len = NetIPv6_CreateIF_ID(if_nbr,                      // Create IF ID from HW mac addr.
                                   &ipv6_id,
                                   NET_IPv6_ADDR_AUTO_CFG_ID_IEEE_48,
                                   p_err);
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        goto exit_fail_stop_auto;
      }

      total_addr_len = id_len + prefix_len;
      if (total_addr_len > NET_IPv6_ADDR_PREFIX_LEN_MAX) {
        NET_CTR_ERR_INC(Net_ErrCtrs.NDP.RxPrefixAddrCfgFail);
        RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
        goto exit_fail_stop_auto;
      }

      (void)NetIPv6_CreateAddrFromID(&ipv6_id,                  // Create IPv6 addr from IF ID.
                                     p_addr_prefix,
                                     NET_IPv6_ADDR_PREFIX_CUSTOM,
                                     prefix_len,
                                     p_err);
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        goto exit_fail_stop_auto;
      }

      //                                                           ---------------- ADD ADDRESS TO IF -----------------
      cfg_status = NetIPv6_CfgAddrAddHandler(if_nbr,
                                             p_addr_prefix,
                                             prefix_len,
                                             lifetime_valid,
                                             lifetime_preferred,
                                             NET_IP_ADDR_CFG_MODE_AUTO_CFG,
                                             dad_en,
                                             addr_cfg_type,
                                             &p_addrs,
                                             p_err);
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        goto exit_fail_stop_auto;
      }

      switch (cfg_status) {
        case NET_IPv6_ADDR_CFG_STATUS_SUCCEED:
          goto exit_succeed_stop_auto;

        case NET_IPv6_ADDR_CFG_STATUS_IN_PROGRESS:
          break;

        case NET_IPv6_ADDR_CFG_STATUS_FAIL:
        case NET_IPv6_ADDR_CFG_STATUS_DUPLICATE:
        default:
          NET_CTR_ERR_INC(Net_ErrCtrs.NDP.RxPrefixAddrCfgFail);
          RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
          goto exit_fail_stop_auto;
      }
    } else {
#ifdef  NET_IPv6_ADDR_AUTO_CFG_MODULE_EN
      if (auto_state == NET_IPv6_AUTO_CFG_STATE_STARTED_GLOBAL) {
        NET_IF            *p_if;
        NET_IPv6_ADDR_OBJ *p_addr_obj;
        CPU_BOOLEAN       prefix_found = DEF_NO;

        p_if = NetIF_Get(if_nbr, p_err);
        if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
          goto exit;
        }

        SLIST_FOR_EACH_ENTRY(p_if->IP_Obj->IPv6_AddrListPtr, p_addr_obj, NET_IPv6_ADDR_OBJ, ListNode) {
          if ((p_addr_obj->AddrHostPrefixLen == prefix_len)
              && (p_addr_obj->AddrCfgMode == NET_IP_ADDR_CFG_MODE_AUTO_CFG)) {
            prefix_found = NetNDP_IsPrefixCfgdOnAddr(&p_addr_obj->AddrHost,
                                                     p_addr_prefix,
                                                     prefix_len);

            if (prefix_found == DEF_YES) {
              p_addrs = p_addr_obj;
              break;
            }
          }
        }
        if (prefix_found == DEF_YES) {
          goto exit_succeed_stop_auto;
        } else {
          goto exit_fail_stop_auto;
        }
      }
#endif
    }
  }

  PP_UNUSED_PARAM(p_addrs);

  goto exit;

exit_succeed_stop_auto:
#ifdef  NET_IPv6_ADDR_AUTO_CFG_MODULE_EN
  if (auto_state == NET_IPv6_AUTO_CFG_STATE_STARTED_GLOBAL) {
    p_auto_obj->AddrGlobalPtr = &p_addrs->AddrHost;
    NetIPv6_AddrAutoCfgComp(if_nbr, NET_IPv6_ADDR_CFG_STATUS_SUCCEED);
  }
#endif
  goto exit;

exit_fail_stop_auto:
#ifdef  NET_IPv6_ADDR_AUTO_CFG_MODULE_EN
  if ((auto_state == NET_IPv6_AUTO_CFG_STATE_STARTED_GLOBAL)
      || (auto_state == NET_IPv6_AUTO_CFG_STATE_STOPPED)       ) {
    p_auto_obj->AddrGlobalPtr = DEF_NULL;
    NetIPv6_AddrAutoCfgComp(if_nbr, NET_IPv6_ADDR_CFG_STATUS_FAIL);
  }
#endif

exit:
  return;
}

/****************************************************************************************************//**
 *                                           NetNDP_RxPrefixHandler()
 *
 * @brief    (1) Add or Update an Prefix entry in the Prefix list based on received NDP prefix:
 *
 *           - (a) Search Prefix List
 *           - (b) Update or add Prefix entry
 *
 * @param    if_nbr          Interface number the packet was received from.
 *
 * @param    p_addr_prefix   Pointer to received NDP prefix.
 *
 * @param    prefix_len      Length of the received prefix.
 *
 * @param    lifetime_valid  Lifetime of the received prefix.
 *
 * @param    on_link         Indicate if prefix is advertised as being on same link.
 *
 * @param    addr_cfg_auto   Indicate that prefix can be used for Autonomous Address Configuration.
 *
 * @param    p_err           Pointer to variable that will receive the return error code from this function.
 *******************************************************************************************************/
static void NetNDP_RxPrefixHandler(NET_IF_NBR          if_nbr,
                                   const NET_IPv6_ADDR *p_addr_prefix,
                                   CPU_INT08U          prefix_len,
                                   CPU_INT32U          lifetime_valid,
                                   CPU_BOOLEAN         on_link,
                                   CPU_BOOLEAN         addr_cfg_auto,
                                   RTOS_ERR            *p_err)
{
  NET_NDP_PREFIX *p_prefix;
  NET_IPv6_ADDR  addr_masked;
  CPU_INT32U     timeout_ms;

  //                                                               Insure that the received prefix is consistent ...
  //                                                               ... with the prefix length.
  NetIPv6_AddrMaskByPrefixLen(p_addr_prefix, prefix_len, &addr_masked, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    NET_CTR_ERR_INC(Net_ErrCtrs.NDP.RxInvalidPrefix);
    RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
    goto exit;
  }

  //                                                               Calculate timeout tick.
  timeout_ms = lifetime_valid * 1000;

  //                                                               ---------------- SEARCH PREFIX LIST ----------------
  p_prefix = NetNDP_PrefixSrch(if_nbr, &addr_masked, p_err);

  if (p_prefix != DEF_NULL) {                                   // Prefix already in prefix list...
    if (on_link == DEF_YES) {
      if (lifetime_valid != 0) {
        //                                                         ... update prefix timeout.
        if (p_prefix->TmrPtr != DEF_NULL) {
          NetTmr_Set(p_prefix->TmrPtr,
                     NetNDP_PrefixTimeout,
                     timeout_ms);
        } else {
          p_prefix->TmrPtr = NetTmr_Get(NetNDP_PrefixTimeout,
                                        p_prefix,
                                        timeout_ms,
                                        NET_TMR_OPT_NONE,
                                        p_err);
          if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
            goto exit;
          }
        }
      } else {
        NetNDP_RemovePrefixDestCache(if_nbr, &p_prefix->Prefix.Addr[0], p_prefix->PrefixLen);
        NetNDP_PrefixRemove(p_prefix, DEF_YES);
      }
    }
  } else {
    //                                                             Prefix is not in NDP cache ...
    if (lifetime_valid == 0u) {                                 // ... if prefix lifetime = 0, do not add it to cache.
      NET_CTR_ERR_INC(Net_ErrCtrs.NDP.RxInvalidPrefix);
      RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
      goto exit;
    }

    if ((on_link == DEF_NO)                                     // If On-link flag and Autonomous flag are not set, ...
        && (addr_cfg_auto == DEF_NO)) {                         // ... do not add prefix to the cache.
      NET_CTR_ERR_INC(Net_ErrCtrs.NDP.RxInvalidPrefix);
      RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
      goto exit;
    }
    //                                                             Add new prefix to the prefix list.
    p_prefix = NetNDP_PrefixCfg(if_nbr,
                                &addr_masked,
                                prefix_len,
                                DEF_YES,
                                NetNDP_PrefixTimeout,
                                timeout_ms,
                                p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      goto exit;
    }
  }

exit:
  return;
}

/****************************************************************************************************//**
 *                                       NetNDP_PrefixAddrsUpdate()
 *
 * @brief    Update all address on the given interface corresponding to the prefix.
 *
 * @param    if_nbr              Interface number to search for on.
 *
 * @param    p_addr_prefix       Pointer to prefix add.
 *
 * @param    prefix_len          Prefix Length.
 *
 * @param    lifetime_valid      Valid Lifetime received with Prefix Information Option.
 *
 * @param    lifetime_preferred  Preferred Lifetime received with Prefix Information Option.
 *
 * @return   DEF_YES     if one or more address where updated.
 *           DEF_NO      otherwise.
 *
 * @note     (1) Address valid lifetime should be set to 2 hours according rules state in RFC #4862 Section 5.5.3
 *               Router Advertisement Processing. #### NET-779.
 *******************************************************************************************************/
static CPU_BOOLEAN NetNDP_RxPrefixAddrsUpdate(NET_IF_NBR    if_nbr,
                                              NET_IPv6_ADDR *p_addr_prefix,
                                              CPU_INT08U    prefix_len,
                                              CPU_INT32U    lifetime_valid,
                                              CPU_INT32U    lifetime_preferred)
{
  NET_IF            *p_if;
  NET_IPv6_ADDR_OBJ *p_addr_obj;
  CPU_INT32U        timeout_ms;
  CPU_INT32U        remain_ms;
  CPU_BOOLEAN       prefix_found;
  CPU_BOOLEAN       addr_updated = DEF_NO;
  NET_TMR           *p_tmr_pref;
  NET_TMR           *p_tmr_valid;
  RTOS_ERR          local_err;

  //                                                               ------------- VALIDATE IPv6 PREFIX PTR -------------
  if (p_addr_prefix == DEF_NULL) {
    goto exit;
  }

  RTOS_ERR_SET(local_err, RTOS_ERR_NONE);

  p_if = NetIF_Get(if_nbr, &local_err);
  if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  //                                                               -------------- SRCH IF FOR IPv6 ADDR ---------------
  addr_updated = DEF_NO;
  prefix_found = DEF_NO;

  SLIST_FOR_EACH_ENTRY(p_if->IP_Obj->IPv6_AddrListPtr, p_addr_obj, NET_IPv6_ADDR_OBJ, ListNode) {
    if ((p_addr_obj->AddrHostPrefixLen == prefix_len)
        && (p_addr_obj->AddrCfgMode == NET_IP_ADDR_CFG_MODE_AUTO_CFG)) {
      prefix_found = NetNDP_IsPrefixCfgdOnAddr(&p_addr_obj->AddrHost,
                                               p_addr_prefix,
                                               prefix_len);

      if (prefix_found == DEF_YES) {                            // If prefix found, ...
                                                                // ... reset addr valid lifetime.
        addr_updated = DEF_YES;
        //                                                         See Note #1.
        p_tmr_valid = p_addr_obj->ValidLifetimeTmrPtr;
        p_tmr_pref = p_addr_obj->PrefLifetimeTmrPtr;

        if (p_tmr_valid != DEF_NULL) {
          remain_ms = NetTmr_TimeRemainGet(p_tmr_valid);

          timeout_ms = lifetime_valid * 1000;

          if ((timeout_ms > remain_ms)
              || (timeout_ms > NET_NDP_LIFETIME_TIMEOUT_MS)) {
            NetTmr_Set(p_tmr_valid,
                       NetIPv6_AddrValidLifetimeTimeout,
                       timeout_ms);
          } else if (remain_ms >= NET_NDP_LIFETIME_TIMEOUT_MS) {
            NetTmr_Set(p_tmr_valid,
                       NetIPv6_AddrValidLifetimeTimeout,
                       NET_NDP_LIFETIME_TIMEOUT_MS);
          } else {
            continue;                                           // ... adv to IF's next addr.
          }
        } else {
          RTOS_ERR_SET(local_err, RTOS_ERR_NONE);

          p_tmr_valid = NetTmr_Get(NetIPv6_AddrValidLifetimeTimeout,
                                   p_addr_obj,
                                   NET_NDP_LIFETIME_TIMEOUT_MS,
                                   NET_TMR_OPT_NONE,
                                   &local_err);
        }

        timeout_ms = lifetime_preferred * 1000;
        if (p_tmr_pref != DEF_NULL) {
          NetTmr_Set(p_tmr_pref,
                     NetIPv6_AddrPrefLifetimeTimeout,
                     timeout_ms);
        } else {
          RTOS_ERR_SET(local_err, RTOS_ERR_NONE);

          p_tmr_pref = NetTmr_Get(NetIPv6_AddrPrefLifetimeTimeout,
                                  p_addr_obj,
                                  timeout_ms,
                                  NET_TMR_OPT_NONE,
                                  &local_err);
        }
      }
    }
  }

exit:
  return (addr_updated);
}

/****************************************************************************************************//**
 *                                       NetNDP_TxNeighborSolicitation()
 *
 * @brief    Transmit Neighbor Solicitation message.
 *
 * @param    if_nbr          Network interface number to transmit Neighbor Solicitation message.
 *
 * @param    p_addr_src      Pointer to IPv6 source address (see Note #1).
 *
 * @param    p_addr_dest     Pointer to IPv6 destination address.
 *
 * @param    ndp_sol_type    Indicate what upper procedure is performing the Tx of Solicitations :
 *                           NET_NDP_NEIGHBOR_SOL_TYPE_DAD    Duplication Address Detection (DAD)
 *                           NET_NDP_NEIGHBOR_SOL_TYPE_RES    Address Resolution
 *                           NET_NDP_NEIGHBOR_SOL_TYPE_NUD    Neighbor Unreachability Detection
 *
 * @param    p_err           Pointer to variable that will receive the return error code from this function.
 *
 * @note     (1) If IPv6 source address pointer is NULL, the unspecified address is used.
 *******************************************************************************************************/
static void NetNDP_TxNeighborSolicitation(NET_IF_NBR                if_nbr,
                                          NET_IPv6_ADDR             *p_addr_src,
                                          NET_IPv6_ADDR             *p_addr_dest,
                                          NET_NDP_NEIGHBOR_SOL_TYPE ndp_sol_type,
                                          RTOS_ERR                  *p_err)
{
  NET_IPv6_ADDR           addr_unspecified;
  NET_IPv6_ADDR           *p_ndp_addr_src;
  NET_NDP_OPT_HW_ADDR_HDR ndp_opt_hw_addr_hdr;
  CPU_INT16U              data_len;
  CPU_INT08U              hw_addr[NET_IP_HW_ADDR_LEN];
  CPU_INT08U              hw_addr_len;
  CPU_BOOLEAN             dest_mcast;
  void                    *p_data;

  p_ndp_addr_src = p_addr_src;
  if (p_ndp_addr_src == DEF_NULL) {                              // If  src addr ptr is NULL                      ...
    NetIPv6_AddrUnspecifiedSet(&addr_unspecified);               // ... tx neighbor solicitation with unspecified ...
    p_ndp_addr_src = &addr_unspecified;                          // ... src addr.
    p_data = DEF_NULL;
    data_len = 0u;
  } else {
    ndp_opt_hw_addr_hdr.Opt.Type = NET_NDP_OPT_TYPE_ADDR_SRC;
    ndp_opt_hw_addr_hdr.Opt.Len = 1u;

    hw_addr_len = sizeof(hw_addr);
    NetIF_AddrHW_GetHandler(if_nbr, hw_addr, &hw_addr_len, p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      goto exit;
    }

    Mem_Copy(&ndp_opt_hw_addr_hdr.Addr[0],
             &hw_addr[0],
             NET_IP_HW_ADDR_LEN);

    p_data = (void *)&ndp_opt_hw_addr_hdr;
    data_len = sizeof(NET_NDP_OPT_HW_ADDR_HDR);
  }

  if (ndp_sol_type == NET_NDP_NEIGHBOR_SOL_TYPE_DAD
      || ndp_sol_type == NET_NDP_NEIGHBOR_SOL_TYPE_RES) {
    dest_mcast = DEF_YES;                                       // Set destination addr to solicited-node-multicast.
  } else {
    dest_mcast = DEF_NO;
  }

  //                                                               -------------------- TX NDP REQ --------------------
  //                                                               Tx Neighbor Solicitation msg.
  (void)NetICMPv6_TxMsgReqHandler(if_nbr,
                                  NET_ICMPv6_MSG_TYPE_NDP_NEIGHBOR_SOL,
                                  NET_ICMPv6_MSG_CODE_NDP_NEIGHBOR_SOL,
                                  0u,
                                  p_ndp_addr_src,
                                  p_addr_dest,
                                  NET_IPv6_HDR_HOP_LIM_MAX,
                                  dest_mcast,
                                  DEF_NULL,
                                  p_data,
                                  data_len,
                                  p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

exit:
  return;
}

/****************************************************************************************************//**
 *                                       NetNDP_IsPrefixCfgdOnAddr()
 *
 * @brief    Validate if an IPv6 prefix is configured on a specific address.
 *
 * @param    p_addr          Pointer to IPv6 address to validate.
 *
 * @param    p_addr_prefix   Pointer to IPv6 prefix to search for.
 *
 * @param    prefix_len      Prefix length.
 *
 * @return   DEF_YES, if the prefix is configured on the specified address.
 *           DEF_NO,  otherwise.
 *******************************************************************************************************/
static CPU_BOOLEAN NetNDP_IsPrefixCfgdOnAddr(NET_IPv6_ADDR *p_addr,
                                             NET_IPv6_ADDR *p_addr_prefix,
                                             CPU_INT08U    prefix_len)
{
  CPU_INT08U  prefix_octets_nbr;
  CPU_INT08U  id_bits_nbr;
  CPU_INT08U  prefix_mask;
  CPU_INT08U  octet_ix;
  CPU_BOOLEAN prefix_found;

  prefix_octets_nbr = prefix_len / DEF_OCTET_NBR_BITS;          // Calc nbr of octets that contain 8 prefix bits.

  //                                                               Calc nbr of remaining ID bits to not consider.
  id_bits_nbr = DEF_OCTET_NBR_BITS - (prefix_len % DEF_OCTET_NBR_BITS);

  prefix_mask = DEF_OCTET_MASK << id_bits_nbr;                  // Set prefix mask.

  octet_ix = 0u;
  while (octet_ix < prefix_octets_nbr) {
    if (p_addr_prefix->Addr[octet_ix] != p_addr->Addr[octet_ix]) {
      return (DEF_NO);
    }
    octet_ix++;
  }

  prefix_found = ((p_addr_prefix->Addr[octet_ix] & prefix_mask) \
                  == (p_addr->Addr[octet_ix]        & prefix_mask)) ? DEF_YES : DEF_NO;

  return (prefix_found);
}

/****************************************************************************************************//**
 *                                       NetNDP_UpdateDefaultRouter()
 *
 * @brief    (1) Update the Default Router for the given interface:
 *
 *           - (a) Search NDP Default router list for a entry that is link with a neighbor cache
 *                   entry who's state is Reachable are maybe reachable (stale, delay, probe).
 *
 * @param    if_nbr  Interface number of the router.
 *
 * @return   Pointer to the selected default router NDP cache entry.
 *
 * @note     (2) RFC 4861 section 6.3.6 (Default Router Selection) specifies :
 *               "Routers that are reachable or probably reachable (i.e., in any state other than
 *               INCOMPLETE) SHOULD be preferred over routers whose reachability is unknown or suspect
 *               (i.e., in the INCOMPLETE state, or for which no Neighbor Cache entry exists)."
 *******************************************************************************************************/
static NET_NDP_ROUTER *NetNDP_UpdateDefaultRouter(NET_IF_NBR if_nbr)
{
  NET_NDP_ROUTER         *p_router = DEF_NULL;
  NET_NDP_ROUTER         *p_router_tmp = DEF_NULL;
  NET_NDP_NEIGHBOR_CACHE *p_cache_neighbor = DEF_NULL;
  CPU_BOOLEAN            found = DEF_NO;

  SLIST_FOR_EACH_ENTRY(NetNDP_DataPtr->RouterListPtr, p_router, NET_NDP_ROUTER, ListNode) {
    if (p_router->IF_Nbr == if_nbr) {
      p_cache_neighbor = p_router->NDP_CachePtr;
      if (p_cache_neighbor != DEF_NULL) {
        switch (p_cache_neighbor->State) {
          case NET_NDP_CACHE_STATE_REACHABLE:
            p_router_tmp = p_router;
            found = DEF_YES;
            break;

          case NET_NDP_CACHE_STATE_INCOMPLETE:
            break;

          default:
            p_router_tmp = p_router;
            break;
        }
      }
    }

    if (found == DEF_YES) {
      break;
    }
  }

  if (p_router_tmp != DEF_NULL) {
    NetNDP_DataPtr->DfltRouterTbl[if_nbr] = p_router_tmp;
  } else {
    NetNDP_DataPtr->DfltRouterTbl[if_nbr] = DEF_NULL;
  }

  return (p_router_tmp);
}

/****************************************************************************************************//**
 *                                           NetNDP_UpdateDestCache()
 *
 * @brief    (1) Update entry in Destination Cache with same Next-Hop address as the received one:
 *
 *           - (a) Search NDP Destination Cache List
 *           - (b) Update Next-Hop address in NDP destination cache whit new address.
 *
 * @param    if_nbr      Interface number of the interface for the given address.
 *
 * @param    p_addr      Pointer to the Next-Hop IPv6 address to update.
 *
 * @param    p_addr_new  Pointer to the new Next-Hop IPv6 address.
 *******************************************************************************************************/
static void NetNDP_UpdateDestCache(NET_IF_NBR       if_nbr,
                                   const CPU_INT08U *p_addr,
                                   const CPU_INT08U *p_addr_new)
{
  NET_NDP_DEST_CACHE *p_dest_cache;
  CPU_BOOLEAN        mem_same;

  SLIST_FOR_EACH_ENTRY(NetNDP_DataPtr->DestListPtr, p_dest_cache, NET_NDP_DEST_CACHE, ListNode) {
    if (p_dest_cache->IF_Nbr == if_nbr) {
      mem_same = Mem_Cmp(&p_dest_cache->AddrNextHop, p_addr, NET_IPv6_ADDR_SIZE);
      if (mem_same == DEF_YES) {
        Mem_Copy(&p_dest_cache->AddrNextHop, p_addr_new, NET_IPv6_ADDR_SIZE);
      }
    }
  }
}

/****************************************************************************************************//**
 *                                       NetNDP_RemoveAddrDestCache()
 *
 * @brief    (1) Invalidate Entry in Destination Cache with Next-Hop Address corresponding to
 *               given address:
 *
 *           - (a) Search NDP Destination Cache List
 *           - (b) Invalidate Next-Hop address in NDP destination cache when same as given address.
 *
 * @param    if_nbr  Interface number of the interface for the given address.
 *
 * @param    p_addr  Pointer to IPv6 address.
 *******************************************************************************************************/
static void NetNDP_RemoveAddrDestCache(NET_IF_NBR       if_nbr,
                                       const CPU_INT08U *p_addr)
{
  NET_NDP_DEST_CACHE *p_dest_cache;
  CPU_BOOLEAN        mem_same;

  SLIST_FOR_EACH_ENTRY(NetNDP_DataPtr->DestListPtr, p_dest_cache, NET_NDP_DEST_CACHE, ListNode) {
    if (p_dest_cache->IF_Nbr == if_nbr) {
      mem_same = Mem_Cmp(&p_dest_cache->AddrNextHop, p_addr, NET_IPv6_ADDR_SIZE);
      if (mem_same == DEF_YES) {
        Mem_Clr(&p_dest_cache->AddrNextHop, NET_IPv6_ADDR_SIZE);
        p_dest_cache->IsValid = DEF_NO;
        p_dest_cache->OnLink = DEF_NO;
      }
    }
  }
}

/****************************************************************************************************//**
 *                                       NETNDP_RemovePrefixDestCache()
 *
 * @brief    (1) Invalidate Entry in Destination Cache with Next-Hop Address with same given prefix:
 *
 *           - (a) Search NDP Destination Cache List
 *           - (b) Invalidate Next-Hop address in NDP cache when prefix match.
 *
 * @param    if_nbr      Interface number of the interface for the given prefix.
 *
 * @param    p_prefix    Pointer to address prefix.
 *
 * @param    prefix_len  Length of the prefix.
 *******************************************************************************************************/
static void NetNDP_RemovePrefixDestCache(NET_IF_NBR       if_nbr,
                                         const CPU_INT08U *p_prefix,
                                         CPU_INT08U       prefix_len)
{
  NET_NDP_DEST_CACHE *p_dest_cache;
  CPU_BOOLEAN        valid;
  RTOS_ERR           local_err;

  SLIST_FOR_EACH_ENTRY(NetNDP_DataPtr->DestListPtr, p_dest_cache, NET_NDP_DEST_CACHE, ListNode) {
    RTOS_ERR_SET(local_err, RTOS_ERR_NONE);

    if (p_dest_cache->IF_Nbr == if_nbr) {
      valid = NetIPv6_IsAddrAndPrefixLenValid(&p_dest_cache->AddrNextHop,
                                              (NET_IPv6_ADDR *) p_prefix,
                                              prefix_len,
                                              &local_err);
      PP_UNUSED_PARAM(local_err);

      if (valid == DEF_YES) {
        Mem_Clr(&p_dest_cache->AddrNextHop, NET_IPv6_ADDR_SIZE);
        p_dest_cache->IsValid = DEF_NO;
        p_dest_cache->OnLink = DEF_NO;
      }
    }
  }
}

/****************************************************************************************************//**
 *                                       NetNDP_NeighborCacheAddPend()
 *
 * @brief    (1) Add a 'PENDING' NDP cache into the NDP Cache List & transmit an NDP Request :
 *
 *           - (a) Configure NDP cache :
 *               - (1) Get sender protocol sender
 *               - (2) Get default-configured NDP cache
 *               - (3) NDP cache state
 *               - (4) Enqueue transmit buffer to NDP cache queue
 *           - (b) Insert   NDP cache into NDP Cache List
 *           - (c) Transmit NDP Request to resolve NDP cache
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
 *
 *                               See also 'NetNDP_CacheHandler()  Note #2e3'.
 *
 * @note     (3) See RFC #1122, Section 2.3.2.2.
 *
 * @note     (4) Since NDP Layer is the last layer to handle & queue the transmit network
 *                               buffer, it is NOT necessary to increment the network buffer's reference
 *                               counter to include the pending NDP cache buffer queue as a new reference
 *                               to the network buffer.
 *
 * @note     (5) Some buffer controls were previously initialized in NetBuf_Get() when the packet
 *                               was received at the network interface layer.  These buffer controls do NOT need
 *                               to be re-initialized but are shown for completeness.
 *******************************************************************************************************/
static void NetNDP_NeighborCacheAddPend(const NET_BUF    *p_buf,
                                        NET_BUF_HDR      *p_buf_hdr,
                                        const CPU_INT08U *p_addr_protocol,
                                        RTOS_ERR         *p_err)
{
  NET_CACHE_ADDR_NDP      *p_cache_addr_ndp;
  NET_NDP_NEIGHBOR_CACHE  *p_cache_ndp;
  CPU_INT08U              addr_protocol_sender[NET_IPv6_ADDR_SIZE];
  NET_IF_NBR              if_nbr;
  CPU_INT32U              timeout_ms;
  NET_NDP_OPT_HW_ADDR_HDR ndp_opt_hw_addr_hdr;
  CPU_INT08U              hw_addr[NET_IP_HW_ADDR_LEN];
  CPU_INT08U              hw_addr_len;
  RTOS_ERR                local_err;
  CORE_DECLARE_IRQ_STATE;

  //                                                               ------------------ CFG NDP CACHE -------------------
  //                                                               Copy sender protocol addr to net order.
  //                                                               Cfg protocol addr generically from IP addr.
  Mem_Copy(&addr_protocol_sender[0], &p_buf_hdr->IPv6_AddrSrc, NET_IPv6_ADDR_SIZE);

  if_nbr = p_buf_hdr->IF_Nbr;

  CORE_ENTER_ATOMIC();
  timeout_ms = NetNDP_DataPtr->SolicitTimeout_ms;
  CORE_EXIT_ATOMIC();

  p_cache_addr_ndp = NetNDP_NeighborCacheAddEntry(if_nbr,
                                                  0,
                                                  p_addr_protocol,
                                                  addr_protocol_sender,
                                                  timeout_ms,
                                                  NetNDP_SolicitTimeout,
                                                  NET_NDP_CACHE_STATE_INCOMPLETE,
                                                  DEF_NO,
                                                  p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  //                                                               Cfg buf's unlink fnct/obj to NDP cache.
  p_buf_hdr->UnlinkFnctPtr = NetCache_UnlinkBuf;
  p_buf_hdr->UnlinkObjPtr = p_cache_addr_ndp;

#if 0                                                           // Init'd in NetBuf_Get() [see Note #5].
  p_buf_hdr->PrevSecListPtr = DEF_NULL;
  p_buf_hdr->NextSecListPtr = DEF_NULL;
#endif
  //                                                               Q buf to NDP cache (see Note #3).
  p_cache_addr_ndp->TxQ_Head = (NET_BUF *)p_buf;
  p_cache_addr_ndp->TxQ_Tail = (NET_BUF *)p_buf;
  p_cache_addr_ndp->TxQ_Nbr++;

  //                                                               -------------------- TX NDP REQ --------------------
  //                                                               Tx Neighbor Solicitation msg to resolve NDP cache.

  ndp_opt_hw_addr_hdr.Opt.Type = NET_NDP_OPT_TYPE_ADDR_SRC;
  ndp_opt_hw_addr_hdr.Opt.Len = 1u;

  hw_addr_len = sizeof(hw_addr);
  NetIF_AddrHW_GetHandler(if_nbr, hw_addr, &hw_addr_len, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  Mem_Copy(&ndp_opt_hw_addr_hdr.Addr[0],
           &hw_addr[0],
           hw_addr_len);

  RTOS_ERR_SET(local_err, RTOS_ERR_NONE);

  NetNDP_TxNeighborSolicitation(if_nbr,
                                &p_buf_hdr->IPv6_AddrSrc,
                                (NET_IPv6_ADDR *)p_addr_protocol,
                                NET_NDP_NEIGHBOR_SOL_TYPE_RES,
                                &local_err);

  p_cache_ndp = (NET_NDP_NEIGHBOR_CACHE *)p_cache_addr_ndp->ParentPtr;
  p_cache_ndp->ReqAttemptsCtr++;                                // Inc req attempts ctr.

exit:
  return;
}

/****************************************************************************************************//**
 *                                       NetNDP_NeighborCacheAddEntry()
 *
 * @brief    Add new entry to the NDP cache.
 *
 * @param    if_nbr              Interface number for this cache entry.
 *
 * @param    p_addr_hw           Pointer to hardware address.
 *
 * @param    p_addr_ipv6         Pointer to IPv6 address of Neighbor.
 *
 * @param    p_addr_ipv6_sender  Pointer to IPv6 address of the sender.
 *
 * @param    timeout_ms          Timeout value (in milliseconds).
 *
 * @param    timeout_fnct        Pointer to timeout function.
 *
 * @param    cache_state         Neighbor Cache initial state.
 *
 * @param    is_router           Indicate if Neighbor entry is also a router.
 *                               DEF_YES  Neighbor is also a router.
 *                               DEF_NO   Neighbor is not advertising itself as a router.
 *
 * @param    p_err               Pointer to variable that will receive the return error code from this function.
 *
 * @return   Pointer to the NDP cache entry created.
 *******************************************************************************************************/
static NET_CACHE_ADDR_NDP *NetNDP_NeighborCacheAddEntry(NET_IF_NBR       if_nbr,
                                                        const CPU_INT08U *p_addr_hw,
                                                        const CPU_INT08U *p_addr_ipv6,
                                                        const CPU_INT08U *p_addr_ipv6_sender,
                                                        CPU_INT32U       timeout_ms,
                                                        CPU_FNCT_PTR     timeout_fnct,
                                                        CPU_INT08U       cache_state,
                                                        CPU_BOOLEAN      is_router,
                                                        RTOS_ERR         *p_err)
{
  NET_CACHE_ADDR_NDP     *p_cache_addr_ndp = DEF_NULL;
  NET_NDP_NEIGHBOR_CACHE *p_cache_ndp = DEF_NULL;
  CPU_BOOLEAN            timer_en;

  if (timeout_ms == 0) {
    timer_en = DEF_NO;
  } else {
    timer_en = DEF_YES;
  }

  p_cache_addr_ndp = (NET_CACHE_ADDR_NDP *)NetCache_CfgAddrs(NET_CACHE_TYPE_NDP,
                                                             if_nbr,
                                                             (CPU_INT08U *)p_addr_hw,
                                                             NET_IF_HW_ADDR_LEN_MAX,
                                                             (CPU_INT08U *)p_addr_ipv6,
                                                             (CPU_INT08U *)p_addr_ipv6_sender,
                                                             NET_IPv6_ADDR_SIZE,
                                                             timer_en,
                                                             timeout_fnct,
                                                             timeout_ms,
                                                             p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  //                                                               Insert entry into NDP cache list.
  NetCache_Insert((NET_CACHE_ADDR *) p_cache_addr_ndp);

  //                                                               Get parent cache.
  p_cache_ndp = (NET_NDP_NEIGHBOR_CACHE *)((NET_CACHE_ADDR *)p_cache_addr_ndp)->ParentPtr;

  p_cache_ndp->State = cache_state;

  if (is_router == DEF_TRUE) {
    //                                                             Set isRouter Flag to high.
    DEF_BIT_SET(p_cache_ndp->Flags, NET_NDP_CACHE_FLAG_ISROUTER);
  } else {
    DEF_BIT_CLR(p_cache_ndp->Flags, (CPU_INT16U)NET_NDP_CACHE_FLAG_ISROUTER);
  }

exit:
  return(p_cache_addr_ndp);
}

/****************************************************************************************************//**
 *                                       NetNDP_NeighborCacheUpdateEntry()
 *
 * @brief    Update existing entry in the Neighbor cache.
 *
 * @param    p_cacne_addr_ndp    Pointer to entry in Address Cache.
 *
 * @param    p_ndp_opt_hw_addr   Pointer to hw address received in NDP message.
 *******************************************************************************************************/
static void NetNDP_NeighborCacheUpdateEntry(NET_CACHE_ADDR_NDP *p_cache_addr_ndp,
                                            CPU_INT08U         *p_ndp_opt_hw_addr)
{
  NET_NDP_NEIGHBOR_CACHE *p_cache;
  CPU_BOOLEAN            same_hw_addr;
  CPU_INT32U             timeout_ms;
  CORE_DECLARE_IRQ_STATE;

  p_cache = (NET_NDP_NEIGHBOR_CACHE *)p_cache_addr_ndp->ParentPtr;

  same_hw_addr = Mem_Cmp(&p_cache_addr_ndp->AddrHW,
                         p_ndp_opt_hw_addr,
                         NET_IF_HW_ADDR_LEN_MAX);

  if (same_hw_addr == DEF_NO) {                                 // Hw addr in cache isn't the same as the received one.
    Mem_Copy(&p_cache_addr_ndp->AddrHW[0],
             p_ndp_opt_hw_addr,
             NET_IF_HW_ADDR_LEN_MAX);

    CORE_ENTER_ATOMIC();
    timeout_ms = NetNDP_NeighborCacheTimeout_ms;
    CORE_EXIT_ATOMIC();
    //                                                             Set cache tmr to CacheTimeout.
    NetTmr_Set(p_cache->TmrPtr,
               NetNDP_CacheTimeout,
               timeout_ms);
    //                                                             Add state of the entry cache to STALE.
    p_cache->State = NET_NDP_CACHE_STATE_STALE;
  } else {                                                      // Hw addr in cache is the same as the received one.
    CORE_ENTER_ATOMIC();
    timeout_ms = NetNDP_DataPtr->ReachableTimeout_ms;
    CORE_EXIT_ATOMIC();
    //                                                             Set cache tmr to ReachableTimeout.
    NetTmr_Set(p_cache->TmrPtr,
               NetNDP_ReachableTimeout,
               timeout_ms);
    //                                                             Add state of the entry cache to REACHABLE.
    p_cache->State = NET_NDP_CACHE_STATE_REACHABLE;
  }
}

/****************************************************************************************************//**
 *                                           NetNDP_CacheRemoveEntry()
 *
 * @brief    Remove an entry in the NDP Neighbor cache.
 *
 * @param    p_cache     Pointer to the NDP Neighbor entry to remove.
 *
 * @param    tmr_free    Indicate if the neighbor cache timer must be freed.
 *                       DEF_YES         Free timer.
 *                       DEF_NO   Do not free timer.
 *******************************************************************************************************/
static void NetNDP_NeighborCacheRemoveEntry(NET_NDP_NEIGHBOR_CACHE *p_cache,
                                            CPU_BOOLEAN            tmr_free)
{
  NET_CACHE_ADDR_NDP *p_cache_addr_ndp;

  p_cache_addr_ndp = p_cache->CacheAddrPtr;

  NetNDP_RemoveAddrDestCache(p_cache_addr_ndp->IF_Nbr,
                             &p_cache_addr_ndp->AddrProtocol[0]);

  NetCache_Remove((NET_CACHE_ADDR *)p_cache_addr_ndp,           // Clr Addr Cache and free tmr if specified.
                  tmr_free);

  p_cache->TmrPtr = DEF_NULL;
  p_cache->ReqAttemptsCtr = 0u;
  p_cache->State = NET_NDP_CACHE_STATE_NONE;
  p_cache->Flags = NET_CACHE_FLAG_NONE;
}

/****************************************************************************************************//**
 *                                           NetNDP_RouterDfltGet()
 *
 * @brief    Retrieve the default router for the given Interface or if no default router is defined,
 *           get a router in the router list.
 *
 * @param    if_nbr      Interface number on which the packet must be send.
 *
 * @param    p_router    Pointer to variable that will receive the router object found.
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function.
 *
 * @return   DEF_YES, if the router found is the router listed as the default one.
 *           DEF_NO,  otherwise.
 *
 * @note     (1) RFC 4861 section 6.3.6 (Default Router Selection) specifies :
 *               "When no routers on the list are known to be reachable or probably reachable, routers
 *               SHOULD be selected in a round-robin fashion, so that subsequent requests for a
 *               default router do not return the same router until all other routers have been
 *               selected."
 *******************************************************************************************************/
static CPU_BOOLEAN NetNDP_RouterDfltGet(NET_IF_NBR     if_nbr,
                                        NET_NDP_ROUTER **p_router,
                                        RTOS_ERR       *p_err)
{
  NET_NDP_ROUTER *p_router_ix;
  NET_NDP_ROUTER *p_router_cache;
  CPU_BOOLEAN    robin_found;
  CPU_BOOLEAN    rtn_val = DEF_NO;

  if (NetNDP_DataPtr->DfltRouterTbl[if_nbr] != DEF_NULL) {
    *p_router = NetNDP_DataPtr->DfltRouterTbl[if_nbr];
    rtn_val = DEF_YES;
    goto exit;
  }

  robin_found = DEF_NO;
  p_router_ix = DEF_NULL;
  p_router_cache = DEF_NULL;

  SLIST_FOR_EACH_ENTRY(NetNDP_DataPtr->RouterListPtr, p_router_ix, NET_NDP_ROUTER, ListNode) {
    if (p_router_ix->IF_Nbr == if_nbr) {
      //                                                           Cache first good router in case round-robin has ..
      //                                                           ... not been assigned.
      if (p_router_cache == DEF_NULL) {
        p_router_cache = p_router_ix;
      }

      //                                                           Find if router has been assigned with round robin.
      if (p_router_ix->RoundRobin == DEF_YES) {
        p_router_ix->RoundRobin = DEF_NO;
        robin_found = DEF_YES;
      } else {
        if (robin_found == DEF_YES) {
          p_router_cache = p_router_ix;
          break;
        }
      }
    }
  }

  if (robin_found == DEF_YES) {                                 // A router assigned with round-robin was found.
    *p_router = p_router_cache;
  } else if (p_router_cache != DEF_NULL) {                      // No router was assign with the round robin...
    p_router_cache->RoundRobin = DEF_YES;                       // ... assign first find in list.
    *p_router = p_router_cache;
  } else {                                                      // No router was found.
    *p_router = DEF_NULL;
    RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_FOUND);
  }

  rtn_val = DEF_NO;

exit:
  return (rtn_val);
}

/****************************************************************************************************//**
 *                                           NetNDP_RouterCfg()
 *
 * @brief    (1) Get and Configure a Router from the router pool.
 *
 *           - (a) Get a router from the router pool and insert it in router list.
 *           - (b) Configure router with received arguments.
 *
 * @param    if_nbr          Number of the Interface that is on same link than router.
 *
 * @param    p_addr          Pointer to router's IPv6 address.
 *
 * @param    timer_en        Indicate whether are not to set a network timer for the router:
 *                           DEF_YES                Set network timer for router.
 *                           DEF_NO          Do NOT set network timer for router.
 *
 * @param    timeout_fnct    Pointer to timeout function.
 *
 * @param    timeout_ms      Timeout value (in milliseconds).
 *
 * @param    p_err           Pointer to variable that will receive the return error code from this function.
 *
 * @return   Pointer to configured router entry.
 *******************************************************************************************************/
static NET_NDP_ROUTER *NetNDP_RouterCfg(NET_IF_NBR    if_nbr,
                                        NET_IPv6_ADDR *p_addr,
                                        CPU_BOOLEAN   timer_en,
                                        CPU_FNCT_PTR  timeout_fnct,
                                        CPU_INT32U    timeout_ms,
                                        RTOS_ERR      *p_err)
{
  NET_NDP_ROUTER *p_router = DEF_NULL;

  p_router = NetNDP_RouterGet(p_err);
  if (p_router == DEF_NULL) {
    goto exit;                                                  // Return err from NetNDP_RouterGet().
  }

  if (timer_en == DEF_YES) {
    p_router->TmrPtr = NetTmr_Get(timeout_fnct,
                                  p_router,
                                  timeout_ms,
                                  NET_TMR_OPT_NONE,
                                  p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {           // If timer unavailable, ...
                                                                // ... free Router.
      NetNDP_RouterRemove((NET_NDP_ROUTER *)p_router, DEF_NO);
      goto exit;
    }
  } else {
    p_router->TmrPtr = DEF_NULL;
  }

  //                                                               ---------------- CFG ROUTER ENTRY ------------------
  p_router->IF_Nbr = if_nbr;

  p_router->RoundRobin = DEF_NO;

  p_router->LifetimeSec = 0u;

  Mem_Copy(&p_router->Addr, p_addr, NET_IPv6_ADDR_SIZE);

  p_router->NDP_CachePtr = DEF_NULL;

exit:
  return (p_router);
}

/****************************************************************************************************//**
 *                                           NetNDP_RouterSrch()
 *
 * @brief    Search for a matching Router for the given address into the Default Router list.
 *
 * @param    if_nbr  Interface number of the router to look for.
 *
 * @param    p_addr  Pointer to the ipv6 address to look for in the Default Router list.
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function.
 *
 * @return   Pointer to router entry found in router list.
 *******************************************************************************************************/
static NET_NDP_ROUTER *NetNDP_RouterSrch(NET_IF_NBR    if_nbr,
                                         NET_IPv6_ADDR *p_addr,
                                         RTOS_ERR      *p_err)
{
  NET_NDP_ROUTER *p_router = DEF_NULL;
  CPU_BOOLEAN    found = DEF_NO;

  SLIST_FOR_EACH_ENTRY(NetNDP_DataPtr->RouterListPtr, p_router, NET_NDP_ROUTER, ListNode) {
    if (p_router->IF_Nbr == if_nbr) {
      //                                                           Cmp address with Router address.
      found = Mem_Cmp(p_addr,
                      &p_router->Addr.Addr[0],
                      NET_IPv6_ADDR_SIZE);

      if (found == DEF_YES) {                                   // If a router is found, ..
        break;                                                  // .. return found Router.
      }
    }
  }

  if (found == DEF_NO) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_FOUND);
    return (DEF_NULL);
  }

  return (p_router);
}

/****************************************************************************************************//**
 *                                           NetNDP_RouterGet()
 *
 * @brief    (1) Get a Router entry.
 *
 *           - (a) Get a router entry from the router pool.
 *           - (b) Insert router entry in the Default Router list.
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function.
 *
 * @return   Pointer to router entry.
 *******************************************************************************************************/
static NET_NDP_ROUTER *NetNDP_RouterGet(RTOS_ERR *p_err)
{
  NET_NDP_ROUTER *p_router = DEF_NULL;
#if (NET_STAT_POOL_NDP_EN == DEF_ENABLED)
  RTOS_ERR local_err;
#endif

  //                                                               ----------- GET ROUTER OBJECT FROM POOL ------------
  p_router = (NET_NDP_ROUTER *)Mem_DynPoolBlkGet(&NetNDP_DataPtr->RouterPool, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (DEF_NULL);
  }

  //                                                               ---------------- ADD ROUTER TO LIST ----------------
  SList_Push(&NetNDP_DataPtr->RouterListPtr, &p_router->ListNode);

#if (NET_STAT_POOL_NDP_EN == DEF_ENABLED)
  //                                                               ------------ UPDATE ROUTER POOL STATS --------------
  RTOS_ERR_SET(local_err, RTOS_ERR_NONE);
  NetStat_PoolEntryUsedInc(&NetNDP_DataPtr->RouterPoolStat, &local_err);
#endif

  return (p_router);
}

/****************************************************************************************************//**
 *                                           NetNDP_RouterRemove()
 *
 * @brief    (1) Remove a router from the router list.
 *
 *           - (a) Unlink router entry from router list.
 *           - (b) Free router entry.
 *
 * @param    p_router    Pointer to the Router entry to insert into the router list.
 *
 * @param    tmr_free    Indicate whether to free network timer :
 *                       DEF_YES                Free network timer for prefix.
 *                       DEF_NO          Do NOT free network timer for prefix.
 *                       [Freed by NetTmr_Handler()].
 *******************************************************************************************************/
static void NetNDP_RouterRemove(NET_NDP_ROUTER *p_router,
                                CPU_BOOLEAN    tmr_free)
{
  RTOS_ERR local_err;

  //                                                               ------------- REMOVE ROUTER FROM LIST --------------
  SList_Rem(&NetNDP_DataPtr->RouterListPtr, &p_router->ListNode);

  //                                                               ----------------- FREE ROUTER TMR ------------------
  if (tmr_free == DEF_YES) {
    if (p_router->TmrPtr != DEF_NULL) {
      NetTmr_Free(p_router->TmrPtr);
      p_router->TmrPtr = DEF_NULL;
    }
  }

  //                                                               ---------------- CLEAR ROUTER ENTRY ----------------
  p_router->NDP_CachePtr = DEF_NULL;

#if (NET_DBG_CFG_MEM_CLR_EN == DEF_ENABLED)
  NetNDP_RouterClr(p_router);
#endif

  //                                                               ---------------- FREE ROUTER OBJECT ----------------
  Mem_DynPoolBlkFree(&NetNDP_DataPtr->RouterPool, p_router, &local_err);
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );

#if (NET_STAT_POOL_NDP_EN == DEF_ENABLED)
  //                                                               ------------- UPDATE ROUTER POOL STATS --------------
  RTOS_ERR_SET(local_err, RTOS_ERR_NONE);
  NetStat_PoolEntryUsedDec(&NetNDP_DataPtr->RouterPoolStat, &local_err);
#endif
}

/****************************************************************************************************//**
 *                                           NetNDP_RouterClr()
 *
 * @brief    Clear a router entry.
 *
 * @param    p_router    Pointer to the Router entry to clear.
 *******************************************************************************************************/

#if (NET_DBG_CFG_MEM_CLR_EN == DEF_ENABLED)
static void NetNDP_RouterClr(NET_NDP_ROUTER *p_router)
{
  p_router->IF_Nbr = NET_IF_NBR_NONE;

  p_router->NDP_CachePtr = DEF_NULL;

  Mem_Clr(&p_router->Addr, NET_IPv6_ADDR_SIZE);

  p_router->LifetimeSec = 0u;

  if (p_router->TmrPtr != DEF_NULL) {
    NetTmr_Free(p_router->TmrPtr);
    p_router->TmrPtr = DEF_NULL;
  }
}
#endif

/****************************************************************************************************//**
 *                                       NetNDP_PrefixSrchMatchAddr()
 *
 * @brief    Search the prefix list for a match with the given address.
 *
 * @param    if_nbr  Interface number of the prefix to look for.
 *
 * @param    p_addr  Pointer to the ipv6 address to match with a prefix in the Prefix list.
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function.
 *
 * @return   Pointer to prefix entry found in list.
 *******************************************************************************************************/
static NET_NDP_PREFIX *NetNDP_PrefixSrchMatchAddr(NET_IF_NBR          if_nbr,
                                                  const NET_IPv6_ADDR *p_addr,
                                                  RTOS_ERR            *p_err)
{
  NET_NDP_PREFIX *p_prefix = DEF_NULL;
  NET_IPv6_ADDR  addr_masked;
  CPU_BOOLEAN    found = DEF_NO;

  SLIST_FOR_EACH_ENTRY(NetNDP_DataPtr->PrefixListPtr, p_prefix, NET_NDP_PREFIX, ListNode) {
    if (p_prefix->IF_Nbr == if_nbr) {
      NetIPv6_AddrMaskByPrefixLen(p_addr, p_prefix->PrefixLen, &addr_masked, p_err);
      //                                                           Compare Prefix with prefix in list.
      found = Mem_Cmp(&addr_masked,
                      &p_prefix->Prefix.Addr[0],
                      NET_IPv6_ADDR_SIZE);

      if (found == DEF_YES) {                                   // If Prefix found, ..
        break;
      }
    }
  }

  if (found != DEF_YES) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_FOUND);
    return (DEF_NULL);
  }

  return (p_prefix);
}

/****************************************************************************************************//**
 *                                           NetNDP_PrefixCfg()
 *
 * @brief    (1) Get and Configured a prefix from the Prefix pool.
 *
 *           - (a) Get a prefix from the prefix pool and insert it in prefix list.
 *           - (b) Configure prefix with received arguments.
 *
 * @param    if_nbr          Interface number of the prefix.
 *
 * @param    p_addr_prefix   Pointer to IPv6 prefix.
 *
 * @param    prefix_len      Length of the prefix to configure.
 *
 * @param    timer_en        Indicate whether are not to set a network timer for the prefix:
 *                           DEF_YES                Set network timer for prefix.
 *                           DEF_NO          Do NOT set network timer for prefix.
 *
 * @param    timeout_fnct    Pointer to timeout function.
 *
 * @param    timeout_ms      Timeout value (in milliseconds).
 *
 * @param    p_err           Pointer to variable that will receive the return error code from this function.
 *
 * @return   Pointer to configured prefix entry.
 *******************************************************************************************************/
static NET_NDP_PREFIX *NetNDP_PrefixCfg(NET_IF_NBR          if_nbr,
                                        const NET_IPv6_ADDR *p_addr_prefix,
                                        CPU_INT08U          prefix_len,
                                        CPU_BOOLEAN         timer_en,
                                        CPU_FNCT_PTR        timeout_fnct,
                                        CPU_INT32U          timeout_ms,
                                        RTOS_ERR            *p_err)
{
  NET_NDP_PREFIX *p_prefix = DEF_NULL;
  NET_NDP_PREFIX *p_prefix_rtn = DEF_NULL;

  p_prefix = NetNDP_PrefixGet(p_err);
  if (p_prefix == DEF_NULL) {
    goto exit;                                                  // Return err from NetNDP_PrefixGet().
  }

  //                                                               ----------------- CFG PREFIX TIMER -----------------
  if (timer_en == DEF_YES) {
    p_prefix->TmrPtr = NetTmr_Get(timeout_fnct,
                                  p_prefix,
                                  timeout_ms,
                                  NET_TMR_OPT_NONE,
                                  p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {           // If timer unavailable, ...
                                                                // ... free Prefix.
      NetNDP_PrefixRemove(p_prefix, DEF_NO);
      goto exit;
    }
  }

  //                                                               ---------------- CFG PREFIX ENTRY ------------------
  p_prefix_rtn = p_prefix;
  p_prefix->IF_Nbr = if_nbr;
  p_prefix->PrefixLen = prefix_len;

  Mem_Copy(&p_prefix->Prefix, p_addr_prefix, NET_IPv6_ADDR_SIZE);

exit:
  return (p_prefix_rtn);
}

/****************************************************************************************************//**
 *                                           NetNDP_PrefixSrch()
 *
 * @brief    Search the prefix list for a match with the given prefix.
 *
 * @param    if_nbr          Interface number of the prefix to look for.
 *
 * @param    p_addr_prefix   Pointer to the ipv6 prefix to look for in the Prefix list.
 *
 * @param    p_err           Pointer to variable that will receive the return error code from this function.
 *
 * @return   Pointer to prefix entry found in list.
 *******************************************************************************************************/
static NET_NDP_PREFIX *NetNDP_PrefixSrch(NET_IF_NBR          if_nbr,
                                         const NET_IPv6_ADDR *p_addr_prefix,
                                         RTOS_ERR            *p_err)
{
  NET_NDP_PREFIX *p_prefix = DEF_NULL;
  CPU_BOOLEAN    found = DEF_NO;

  SLIST_FOR_EACH_ENTRY(NetNDP_DataPtr->PrefixListPtr, p_prefix, NET_NDP_PREFIX, ListNode) {
    if (p_prefix->IF_Nbr == if_nbr) {
      //                                                           Compare prefixes.
      found = Mem_Cmp(p_addr_prefix,
                      &p_prefix->Prefix.Addr[0],
                      NET_IPv6_ADDR_SIZE);
      if (found == DEF_YES) {                                   // If Prefix found, return prefix.
        break;
      }
    }
  }

  if (found != DEF_YES) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_FOUND);
    return (DEF_NULL);
  }

  return (p_prefix);
}

/****************************************************************************************************//**
 *                                           NetNDP_PrefixGet()
 *
 * @brief    (1) Get a Prefix entry.
 *
 *           - (a) Get a prefix entry from the prefix pool.
 *           - (b) Insert prefix in the prefix list.
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function.
 *
 * @return   Pointer to prefix entry.
 *******************************************************************************************************/
static NET_NDP_PREFIX *NetNDP_PrefixGet(RTOS_ERR *p_err)
{
  NET_NDP_PREFIX *p_prefix = DEF_NULL;
#if (NET_STAT_POOL_NDP_EN == DEF_ENABLED)
  RTOS_ERR local_err;
#endif

  p_prefix = (NET_NDP_PREFIX *)Mem_DynPoolBlkGet(&NetNDP_DataPtr->PrefixPool, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (DEF_NULL);
  }

  SList_Push(&NetNDP_DataPtr->PrefixListPtr, &p_prefix->ListNode);

#if (NET_STAT_POOL_NDP_EN == DEF_ENABLED)
  //                                                               ------------ UPDATE PREFIX POOL STATS --------------
  RTOS_ERR_SET(local_err, RTOS_ERR_NONE);
  NetStat_PoolEntryUsedInc(&NetNDP_DataPtr->PrefixPoolStat, &local_err);
#endif

  return (p_prefix);
}

/****************************************************************************************************//**
 *                                           NetNDP_PrefixRemove()
 *
 * @brief    (1) Remove a prefix from the prefix list.
 *
 *           - (a) Unlink prefix from prefix list.
 *           - (b) Free prefix entry.
 *
 * @param    p_prefix    Pointer to the Prefix entry to remove from the Prefix list.
 *
 * @param    tmr_free    Indicate whether to free network timer :
 *                       DEF_YES                Free network timer for prefix.
 *                       DEF_NO          Do NOT free network timer for prefix.
 *                       [Freed by NetTmr_Handler()].
 *******************************************************************************************************/
static void NetNDP_PrefixRemove(NET_NDP_PREFIX *p_prefix,
                                CPU_BOOLEAN    tmr_free)
{
  RTOS_ERR local_err;

  SList_Rem(&NetNDP_DataPtr->PrefixListPtr, &p_prefix->ListNode);

  //                                                               ----------------- FREE PREFIX TMR ------------------
  if (tmr_free == DEF_YES) {
    if (p_prefix->TmrPtr != DEF_NULL) {
      NetTmr_Free(p_prefix->TmrPtr);
      p_prefix->TmrPtr = DEF_NULL;
    }
  }

  //                                                               ---------------- CLEAR PREFIX ENTRY ----------------
#if (NET_DBG_CFG_MEM_CLR_EN == DEF_ENABLED)
  NetNDP_PrefixClr(p_prefix);
#endif

  //                                                               ---------------- FREE PREFIX ENTRY -----------------
  Mem_DynPoolBlkFree(&NetNDP_DataPtr->PrefixPool, p_prefix, &local_err);
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );

#if (NET_STAT_POOL_NDP_EN == DEF_ENABLED)
  //                                                               ------------- UPDATE PREFIX POOL STATS --------------
  RTOS_ERR_SET(local_err, RTOS_ERR_NONE);
  NetStat_PoolEntryUsedDec(&NetNDP_DataPtr->PrefixPoolStat, &local_err);
#endif
}

/****************************************************************************************************//**
 *                                           NetNDP_PrefixClr()
 *
 * @brief    Clear a Prefix entry.
 *
 * @param    p_prefix    Pointer to the Prefix entry to clear.
 *******************************************************************************************************/

#if (NET_DBG_CFG_MEM_CLR_EN == DEF_ENABLED)
static void NetNDP_PrefixClr(NET_NDP_PREFIX *p_prefix)
{
  p_prefix->IF_Nbr = NET_IF_NBR_NONE;

  Mem_Clr(&p_prefix->Prefix, NET_IPv6_ADDR_SIZE);

  if (p_prefix->TmrPtr != DEF_NULL) {
    NetTmr_Free(p_prefix->TmrPtr);
    p_prefix->TmrPtr = DEF_NULL;
  }
}
#endif

/****************************************************************************************************//**
 *                                           NetNDP_DestCacheCfg()
 *
 * @brief    (1) Get and Configure a Destination entry from the Destination pool.
 *
 *           - (a) Get a Destination from the destination pool and insert it in destination cache.
 *           - (b) Configure destination with received arguments.
 *
 * @param    if_nbr              Interface number for the destination to configure.
 *
 * @param    p_addr_dest         Pointer to IPv6 Destination address.
 *
 * @param    p_addr_next_hop     Pointer to Next-Hop IPv6 address.
 *
 * @param    is_valid            Indicate whether are not the Next-Hop address is valid.
 *                               DEF_YES, address is   valid
 *                               DEF_NO,  address is invalid
 *
 * @param    on_link             Indicate whether are not the Destination is on link.
 *                               DEF_YES, destination is     on link
 *                               DEF_NO,  destination is not on link
 *
 * @param    p_err               Pointer to variable that will receive the return error code from this function.
 *
 *
 *               p_err       Pointer to variable that will receive the return error code from this function.
 *
 * Return(s)   : Pointer to destination entry configured.
 *
 * Note(s)     : none.
 *******************************************************************************************************/
static NET_NDP_DEST_CACHE *NetNDP_DestCacheCfg(NET_IF_NBR          if_nbr,
                                               const NET_IPv6_ADDR *p_addr_dest,
                                               const NET_IPv6_ADDR *p_addr_next_hop,
                                               CPU_BOOLEAN         is_valid,
                                               CPU_BOOLEAN         on_link,
                                               RTOS_ERR            *p_err)
{
  NET_NDP_DEST_CACHE *p_dest = DEF_NULL;

  p_dest = NetNDP_DestCacheGet(p_err);
  if (p_dest == DEF_NULL) {
    goto exit;                                                  // Return err from NetNDP_DestGet().
  }

  //                                                               -------------- CFG DESTINATION ENTRY ---------------
  p_dest->IF_Nbr = if_nbr;
  p_dest->IsValid = is_valid;
  p_dest->OnLink = on_link;

  Mem_Copy(&p_dest->AddrDest, p_addr_dest, NET_IPv6_ADDR_SIZE);

  if (p_addr_next_hop != DEF_NULL) {
    Mem_Copy(&p_dest->AddrNextHop, p_addr_next_hop, NET_IPv6_ADDR_SIZE);
  }

exit:
  return (p_dest);
}

/****************************************************************************************************//**
 *                                           NetNDP_DestCacheGet()
 *
 * @brief    (1) Get a Destination cache entry from the Destination pool.
 *
 *           - (a) Get a free destination entry from pool.
 *           - (b) Insert destination into cache list.
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function.
 *
 * @return   Pointer to Destination Cache entry.
 *******************************************************************************************************/
static NET_NDP_DEST_CACHE *NetNDP_DestCacheGet(RTOS_ERR *p_err)
{
  NET_NDP_DEST_CACHE *p_dest = DEF_NULL;
#if (NET_STAT_POOL_NDP_EN == DEF_ENABLED)
  RTOS_ERR local_err;
#endif

  p_dest = (NET_NDP_DEST_CACHE *)Mem_DynPoolBlkGet(&NetNDP_DataPtr->DestPool, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    p_dest = NetNDP_DestCacheSrchInvalid(p_err);                // ... find an invalid Destination cache to remove ...
    if (p_dest == DEF_NULL) {
      SLIST_MEMBER *p_node;

      //                                                           ... if none avail, get Dest entry from list start.
      p_node = SList_Pop(&NetNDP_DataPtr->DestListPtr);
      if (p_node == DEF_NULL) {
        RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_FOUND);
        return (DEF_NULL);
      }

      p_dest = SLIST_ENTRY(p_node, NET_NDP_DEST_CACHE, ListNode);
    }
  }

  //                                                               -------------- INIT DESTINATION ENTRY --------------
  NetNDP_DestCacheClr(p_dest);

#if (NET_STAT_POOL_NDP_EN == DEF_ENABLED)
  //                                                               --------- UPDATE DESTINATION POOL STATS ------------
  RTOS_ERR_SET(local_err, RTOS_ERR_NONE);
  NetStat_PoolEntryUsedInc(&NetNDP_DataPtr->DestPoolStat, &local_err);
#endif

  //                                                               ---- INSERT DESTINATION INTO DESTINATION CACHE -----
  SList_PushBack(&NetNDP_DataPtr->DestListPtr, &p_dest->ListNode);

  return (p_dest);
}

/****************************************************************************************************//**
 *                                           NetNDP_DestCacheSrch()
 *
 * @brief    Search for a matching Destination entry for the given address into the Destination Cache.
 *
 * @param    if_nbr  Interface number of the destination to look for.
 *
 * @param    p_addr  Pointer to the ipv6 address to look for in the Destination Cache.
 *
 * @return   Pointer to destination entry found in destination cache.
 *******************************************************************************************************/
static NET_NDP_DEST_CACHE *NetNDP_DestCacheSrch(NET_IF_NBR          if_nbr,
                                                const NET_IPv6_ADDR *p_addr)
{
  NET_NDP_DEST_CACHE *p_dest = DEF_NULL;
  CPU_BOOLEAN        found = DEF_NO;

  SLIST_FOR_EACH_ENTRY(NetNDP_DataPtr->DestListPtr, p_dest, NET_NDP_DEST_CACHE, ListNode) {
    if (p_dest->IF_Nbr == if_nbr) {
      //                                                           Cmp address with Destination cache address.
      found = Mem_Cmp(p_addr,
                      &p_dest->AddrDest.Addr[0],
                      NET_IPv6_ADDR_SIZE);

      if (found == DEF_YES) {                                   // If Destination Cache found, ...
        break;                                                  // ... rtn found Destination cache.
      }
    }
  }

  if (found != DEF_YES) {
    p_dest = DEF_NULL;
  }

  return (p_dest);
}

/****************************************************************************************************//**
 *                                       NetNDP_DestCacheSrchInvalid()
 *
 * @brief    Search for an invalid destination cache entry in the destination cache and remove it from
 *           the list.
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function:
 *
 * @return   Pointer to Destination Cache entry found.
 *******************************************************************************************************/
static NET_NDP_DEST_CACHE *NetNDP_DestCacheSrchInvalid(RTOS_ERR *p_err)
{
  NET_NDP_DEST_CACHE *p_dest_cache = DEF_NULL;
  CPU_BOOLEAN        found = DEF_NO;

  SLIST_FOR_EACH_ENTRY(NetNDP_DataPtr->DestListPtr, p_dest_cache, NET_NDP_DEST_CACHE, ListNode) {
    if (p_dest_cache->IsValid == DEF_NO) {
      //                                                           If Destination Cache found, ...
      found = DEF_YES;                                          // ... rtn found Destination cache.
      break;
    }
  }

  if (found != DEF_YES) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_FOUND);
    return (DEF_NULL);
  }

  SList_Rem(&NetNDP_DataPtr->DestListPtr, &p_dest_cache->ListNode);

  return (p_dest_cache);
}

/****************************************************************************************************//**
 *                                           NetNDP_DestCacheRemove()
 *
 * @brief    (1) Remove a Destination cache entry from the Destination Cache.
 *
 *           - (a) Unlink destination entry from list
 *           - (b) Free destination entry
 *
 *
 * Argument(s) : p_dest    Pointer to the Destination entry to remove from the Destination Cache list.
 *
 * Return(s)   : none.
 *
 * Note(s)     : none.
 *******************************************************************************************************/
static void NetNDP_DestCacheRemove(NET_NDP_DEST_CACHE *p_dest)
{
  RTOS_ERR local_err;

  //                                                               -------- REMOVE DESTINATION CACHE FROM LIST --------
  SList_Rem(&NetNDP_DataPtr->DestListPtr, &p_dest->ListNode);

  //                                                               -------------- CLEAR DESTINATION ENTRY -------------
#if (NET_DBG_CFG_MEM_CLR_EN == DEF_ENABLED)
  NetNDP_DestCacheClr(p_dest);
#endif

  //                                                               -------------- FREE DESTINATION ENTRY --------------
  Mem_DynPoolBlkFree(&NetNDP_DataPtr->DestPool, p_dest, &local_err);
  RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );

#if (NET_STAT_POOL_NDP_EN == DEF_ENABLED)
  //                                                               -------- UPDATE DESTINATION ENTRY POOL STATS --------
  RTOS_ERR_SET(local_err, RTOS_ERR_NONE);
  NetStat_PoolEntryUsedDec(&NetNDP_DataPtr->DestPoolStat, &local_err);
#endif
}

/****************************************************************************************************//**
 *                                           NetNDP_DestCacheClr()
 *
 * @brief    Clear a Destination cache entry.
 *
 * @param    p_dest  Pointer to the Destination entry to clear.
 *******************************************************************************************************/
static void NetNDP_DestCacheClr(NET_NDP_DEST_CACHE *p_dest)
{
  p_dest->IF_Nbr = NET_IF_NBR_NONE;

  p_dest->IsValid = DEF_NO;
  p_dest->OnLink = DEF_NO;

  Mem_Clr(&p_dest->AddrDest, NET_IPv6_ADDR_SIZE);
  Mem_Clr(&p_dest->AddrNextHop, NET_IPv6_ADDR_SIZE);
}

/****************************************************************************************************//**
 *                                           NetNDP_IsAddrOnLink()
 *
 * @brief    Validate if an IPv6 address is on-link or not.
 *
 * @param    if_nbr  Interface number of the address to validate.
 *
 * @param    p_addr  Pointer to IPv6 address to validate.
 *
 * @return   DEF_YES, if IPv6 address is on-link.
 *           DEF_NO,  otherwise.
 *
 * @note     (1) A node considers an IPv6 address to be on-link if that addresses satisfied one of
 *               the following conditions:
 *
 *           - (a) The address is covered by one of the on-link prefixes assigned to the link.
 *
 *           - (b) The address is the target address of a Redirect message sent by a router.
 *******************************************************************************************************/
static CPU_BOOLEAN NetNDP_IsAddrOnLink(NET_IF_NBR          if_nbr,
                                       const NET_IPv6_ADDR *p_addr)
{
  NET_NDP_PREFIX *p_prefix;
  CPU_BOOLEAN    addr_linklocal;
  CPU_BOOLEAN    rtn_val = DEF_NO;
  RTOS_ERR       local_err;

  RTOS_ERR_SET(local_err, RTOS_ERR_NONE);

  //                                                               -------------- VALIDATE IPv6 ADDR PTR --------------
  if (p_addr == DEF_NULL) {
    rtn_val = DEF_NO;
    goto exit;
  }
  //                                                               A link-local addr is always consider on link.
  addr_linklocal = NetIPv6_IsAddrLinkLocal(p_addr);
  if (addr_linklocal == DEF_TRUE) {
    rtn_val = DEF_YES;
    goto exit;
  }

  //                                                               Srch prefix list for a matching prefix to the addr.
  p_prefix = NetNDP_PrefixSrchMatchAddr(if_nbr,
                                        p_addr,
                                        &local_err);
  if (p_prefix != DEF_NULL) {
    rtn_val = DEF_YES;
    goto exit;
  }

  rtn_val = DEF_NO;

exit:
  return (rtn_val);
}

/****************************************************************************************************//**
 *                                           NetNDP_ReachableTimeout()
 *
 * @brief    Change the NDP neighbor cache state from 'REACHABLE' to 'STALE'.
 *
 * @param    p_cache_timeout     Pointer to an NDP neighbor cache.
 *
 * @note     (1) This function is a network timer callback function :
 *
 *                                   - (a) Clear the timer pointer ... :
 *                                   - (1) Cleared in NetNDP_CacheFree() via NetNDP_CacheRemove(); or
 *                                   - (2) Reset   by NetTmr_Get().
 *
 *                                   - (b) but do NOT re-free the timer.
 *******************************************************************************************************/
static void NetNDP_ReachableTimeout(void *p_cache_timeout)
{
  NET_NDP_NEIGHBOR_CACHE *p_cache;
  CPU_INT32U             timeout_ms;
  RTOS_ERR               local_err;
  CORE_DECLARE_IRQ_STATE;

  RTOS_ERR_SET(local_err, RTOS_ERR_NONE);

  p_cache = (NET_NDP_NEIGHBOR_CACHE *)p_cache_timeout;

  p_cache->TmrPtr = DEF_NULL;

  p_cache->State = NET_NDP_CACHE_STATE_STALE;
  CORE_ENTER_ATOMIC();
  timeout_ms = NetNDP_NeighborCacheTimeout_ms;
  CORE_EXIT_ATOMIC();

  p_cache->TmrPtr = NetTmr_Get(NetNDP_CacheTimeout,
                               p_cache,
                               timeout_ms,
                               NET_TMR_OPT_NONE,
                               &local_err);
  if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {          // If tmr unavailable, free NDP cache.
    NetNDP_NeighborCacheRemoveEntry(p_cache, DEF_NO);
    goto exit;
  }

exit:
  return;
}

/****************************************************************************************************//**
 *                                           NetNDP_SolicitTimeout()
 *
 * @brief    Retry NDP Solicitation to resolve an NDP neighbor cache in the 'INCOMPLETE' or
 *           "PROBE" state on the NDP Solicitation timeout.
 *
 * @param    p_cache_timeout     Pointer to an NDP neighbor cache.
 *
 * @note     (1) This function is a network timer callback function :
 *
 *                                   - (a) Clear the timer pointer ... :
 *                                   - (1) Cleared in NetNDP_CacheFree() via NetNDP_CacheRemove(); or
 *                                   - (2) Reset   by NetTmr_Get().
 *
 *                                   - (b) but do NOT re-free the timer.
 *
 * @note     (2) See RFC 4861 section 7.7.2.
 *
 *                               NetICMPv6_TxMsgErr() function is not adequate because it assume that p_buf is a
 *                               pointer to a received packet and not a queue of packet waiting to be send.
 *                               Therefore the address destination and address source are inverted when sending
 *                               the error message.
 *                               #### NET-780
 *                               #### NET-781
 *******************************************************************************************************/
static void NetNDP_SolicitTimeout(void *p_cache_timeout)
{
  NET_NDP_ROUTER         *p_router;
  NET_NDP_NEIGHBOR_CACHE *p_cache;
  NET_CACHE_ADDR_NDP     *p_cache_addr_ndp;
#if 0
  NET_BUF     *p_buf_list;
  NET_BUF     *p_buf_list_next;
  NET_BUF     *p_buf;
  NET_BUF     *p_buf_next;
  NET_BUF_HDR *p_buf_hdr;
#endif
  CPU_INT32U                timeout_ms;
  CPU_INT08U                th_max;
  CPU_INT08U                ndp_cache_state;
  NET_NDP_NEIGHBOR_SOL_TYPE ndp_sol_type;
  CPU_BOOLEAN               is_router;
  RTOS_ERR                  local_err;
  CORE_DECLARE_IRQ_STATE;

  p_cache = (NET_NDP_NEIGHBOR_CACHE *)p_cache_timeout;
  p_cache_addr_ndp = p_cache->CacheAddrPtr;

  p_cache->TmrPtr = DEF_NULL;                                   // Un-reference tmr in the NDP cache.

  ndp_cache_state = p_cache->State;
  switch (ndp_cache_state) {
    case NET_NDP_CACHE_STATE_INCOMPLETE:
      CORE_ENTER_ATOMIC();
      th_max = NetNDP_DataPtr->SolicitMaxAttempsMulticastNbr;
      CORE_EXIT_ATOMIC();
      ndp_sol_type = NET_NDP_NEIGHBOR_SOL_TYPE_RES;
      break;

    case NET_NDP_CACHE_STATE_PROBE:
      CORE_ENTER_ATOMIC();
      th_max = NetNDP_DataPtr->SolicitMaxAttempsUnicastNbr;
      CORE_EXIT_ATOMIC();
      ndp_sol_type = NET_NDP_NEIGHBOR_SOL_TYPE_NUD;
      break;

    default:
      goto exit;
  }

  if (p_cache->ReqAttemptsCtr >= th_max) {                      // If nbr attempts >= max, ...
                                                                // ... update dest cache to remove unreachable next-hop
    NetNDP_RemoveAddrDestCache(p_cache_addr_ndp->IF_Nbr,
                               &p_cache_addr_ndp->AddrProtocol[0]);

    //                                                             ... if Neighbor cache is also a router, ...
    is_router = DEF_BIT_IS_SET(p_cache->Flags, NET_NDP_CACHE_FLAG_ISROUTER);
    if (is_router == DEF_YES) {
      RTOS_ERR_SET(local_err, RTOS_ERR_NONE);

      p_router = NetNDP_RouterSrch(p_cache_addr_ndp->IF_Nbr,
                                   (NET_IPv6_ADDR *)&p_cache_addr_ndp->AddrProtocol,
                                   &local_err);
      if (p_router != DEF_NULL) {
        NetNDP_RouterRemove(p_router, DEF_YES);                 // ... delete router in Default router list.
        (void)NetNDP_UpdateDefaultRouter(p_cache_addr_ndp->IF_Nbr);
      }
    }

    //                                                             See Note #2.
#if 0
    if (p_cache->State == NET_NDP_CACHE_STATE_INCOMPLETE) {
      p_buf_list = p_cache_addr_ndp->TxQ_Head;
      while (p_buf_list != DEF_NULL) {
        p_buf_hdr = &p_buf_list->Hdr;
        p_buf_list_next = p_buf_hdr->NextSecListPtr;
        p_buf = p_buf_list;
        while (p_buf != DEF_NULL) {
          p_buf_hdr = &p_buf->Hdr;
          p_buf_next = p_buf_hdr->NextBufPtr;
          RTOS_ERR_SET(local_err, RTOS_ERR_NONE);
          NetICMPv6_TxMsgErr(p_buf_list,
                             NET_ICMPv6_MSG_TYPE_DEST_UNREACH,
                             NET_ICMPv6_MSG_CODE_DEST_ADDR_UNREACHABLE,
                             NET_ICMPv6_MSG_PTR_NONE,
                             &local_err);
          p_buf = p_buf_next;
        }

        p_buf_list = p_buf_list_next;
      }
    }
#endif

    NetNDP_NeighborCacheRemoveEntry(p_cache, DEF_NO);           // ... free NDP cache.

    goto exit;
  }

  //                                                               ------------------ RETRY NDP REQ -------------------
  CORE_ENTER_ATOMIC();
  timeout_ms = NetNDP_DataPtr->SolicitTimeout_ms;
  CORE_EXIT_ATOMIC();

  RTOS_ERR_SET(local_err, RTOS_ERR_NONE);

  p_cache->TmrPtr = NetTmr_Get(NetNDP_SolicitTimeout,
                               p_cache,
                               timeout_ms,
                               NET_TMR_OPT_NONE,
                               &local_err);
  if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {          // If tmr unavail, free NDP cache.
    NetNDP_NeighborCacheRemoveEntry(p_cache, DEF_NO);
    goto exit;
  }

  //                                                               ------------- RE_TX NEIGHBOR SOL MSG ---------------

  RTOS_ERR_SET(local_err, RTOS_ERR_NONE);

  NetNDP_TxNeighborSolicitation(p_cache_addr_ndp->IF_Nbr,
                                (NET_IPv6_ADDR *)p_cache_addr_ndp->AddrProtocolSender,
                                (NET_IPv6_ADDR *)p_cache_addr_ndp->AddrProtocol,
                                ndp_sol_type,
                                &local_err);

  p_cache->ReqAttemptsCtr++;                                    // Inc req attempts ctr.

exit:
  return;
}

/****************************************************************************************************//**
 *                                           NetNDP_RouterTimeout()
 *
 * @brief    (1) Remove Router entry in NDP default router list.
 *
 *           - (a) Remove Next-Hop address in destination cache corresponding to router.
 *           - (b) Remove NDP router entry in default router list.
 *           - (c) Update the Default Router.
 *
 *
 * Argument(s) : p_cache_timeout      Pointer to an NDP router.
 *
 * Return(s)   : none.
 *
 * Note(s)     : (1) This function is a network timer callback function :
 *
 *                   (a) Clear the timer pointer ... :
 *                       (1) Cleared in NetNDP_CacheFree() via NetNDP_CacheRemove(); or
 *                       (2) Reset   by NetTmr_Get().
 *
 *                   (b) but do NOT re-free the timer.
 *******************************************************************************************************/
static void NetNDP_RouterTimeout(void *p_router_timeout)
{
  NET_NDP_ROUTER *p_router;
  NET_IF_NBR     if_nbr;

  p_router = (NET_NDP_ROUTER *)p_router_timeout;

  p_router->TmrPtr = DEF_NULL;

  //                                                               Rem. NextHop in Dest. Cache corresponding to router.
  NetNDP_RemoveAddrDestCache(p_router->IF_Nbr, &p_router->Addr.Addr[0]);

  if_nbr = p_router->IF_Nbr;

  //                                                               Remove Router from the default router list.
  NetNDP_RouterRemove(p_router, DEF_NO);

  //                                                               Update the Default Router.
  (void)NetNDP_UpdateDefaultRouter(if_nbr);
}

/****************************************************************************************************//**
 *                                           NetNDP_PrefixTimeout()
 *
 * @brief    (1) Remove Prefix entry in NDP prefix list.
 *
 *           - (a) Remove Next-Hop address in Destination Cache with corresponding prefix.
 *           - (b) Remove prefix entry from NDP prefix list.
 *
 * @param    p_prefix_timeout    Pointer to NDP prefix entry.
 *
 * @note     (1) This function is a network timer callback function :
 *
 *                                   - (a) Clear the timer pointer ... :
 *                                   - (1) Cleared in NetNDP_CacheFree() via NetNDP_CacheRemove(); or
 *                                   - (2) Reset   by NetTmr_Get().
 *
 *                                   - (b) but do NOT re-free the timer.
 *******************************************************************************************************/
static void NetNDP_PrefixTimeout(void *p_prefix_timeout)
{
  NET_NDP_PREFIX *p_prefix;

  p_prefix = (NET_NDP_PREFIX *)p_prefix_timeout;

  p_prefix->TmrPtr = DEF_NULL;

  //                                                               Remove next-hop with prefix in destination cache ...
  //                                                               ... See RFC 4861 section 5.3.
  NetNDP_RemovePrefixDestCache(p_prefix->IF_Nbr,
                               &p_prefix->Prefix.Addr[0],
                               p_prefix->PrefixLen);

  //                                                               Remove Prefix entry in Prefix list.
  NetNDP_PrefixRemove(p_prefix, DEF_NO);
}

/****************************************************************************************************//**
 *                                           NetNDP_DAD_Timeout()
 *
 * @brief    Retry NDP Request (sending NS) for the Duplication Address Detection (DAD).
 *
 * @param    p_cache_timeout     Pointer to an NDP cache.
 *
 * @note     (2) This function is a network timer callback function :
 *
 *                                   - (a) Clear the timer pointer ... :
 *                                   - (1) Cleared in NetNDP_CacheFree() via NetNDP_CacheRemove(); or
 *                                   - (2) Reset   by NetTmr_Get().
 *
 *                                   - (b) but do NOT re-free the timer.
 *******************************************************************************************************/
#ifdef NET_DAD_MODULE_EN
static void NetNDP_DAD_Timeout(void *p_cache_timeout)
{
  NET_IF_NBR               if_nbr;
  NET_IPv6_ADDR_OBJ        *p_addr_obj;
  NET_IPv6_ADDR            *p_addr;
  NET_NDP_NEIGHBOR_CACHE   *p_cache;
  NET_CACHE_ADDR_NDP       *p_cache_addr_ndp;
  NET_DAD_OBJ              *p_dad_obj;
  CPU_INT32U               timeout_ms;
  CPU_INT08U               th_max;
  NET_IPv6_ADDR_CFG_STATUS status;
  RTOS_ERR                 local_err;
  CORE_DECLARE_IRQ_STATE;

  p_cache = (NET_NDP_NEIGHBOR_CACHE *) p_cache_timeout;

  p_cache_addr_ndp = p_cache->CacheAddrPtr;

  if_nbr = p_cache_addr_ndp->IF_Nbr;

  p_cache->TmrPtr = DEF_NULL;

  p_addr = (NET_IPv6_ADDR *)&p_cache_addr_ndp->AddrProtocol[0];
  p_dad_obj = NetDAD_ObjSrch(p_addr);
  if (p_dad_obj == DEF_NULL) {
    CPU_SW_EXCEPTION();
  }

  CORE_ENTER_ATOMIC();
  th_max = NetNDP_DataPtr->DAD_MaxAttemptsNbr;
  CORE_EXIT_ATOMIC();

  //                                                               --------- VERIFY IF DAD SIGNAL ERR RECEIVED --------
  RTOS_ERR_SET(local_err, RTOS_ERR_NONE);

  NetDAD_SignalWait(NET_DAD_SIGNAL_TYPE_ERR, p_dad_obj, &local_err);
  if (RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE) {
    KAL_SemSet(p_dad_obj->SignalErr, 0, &local_err);            // DAD process failed.
    RTOS_ASSERT_CRITICAL((RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE), RTOS_ERR_ASSERT_CRITICAL_FAIL,; );
    status = NET_IPv6_ADDR_CFG_STATUS_DUPLICATE;
    goto exit_update;
  }

  //                                                               -------- VERIFY IF ALL DAD ATTEMPTS ARE SENT -------
  if (p_cache->ReqAttemptsCtr >= th_max) {
    status = NET_IPv6_ADDR_CFG_STATUS_SUCCEED;                  // DAD process succeeded.
    goto exit_update;
  }

  //                                                               ------------------ RETRY NDP REQ -------------------
  CORE_ENTER_ATOMIC();
  timeout_ms = NetNDP_DataPtr->SolicitTimeout_ms;
  CORE_EXIT_ATOMIC();

  //                                                               Get new timer for NDP cache.
  p_cache->TmrPtr = NetTmr_Get(NetNDP_DAD_Timeout,
                               p_cache,
                               timeout_ms,
                               NET_TMR_OPT_NONE,
                               &local_err);
  if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {          // If timer unavailable, free NDP cache.
    status = NET_IPv6_ADDR_CFG_STATUS_FAIL;
    goto exit_update;
  }

  RTOS_ERR_SET(local_err, RTOS_ERR_NONE);

  //                                                               Transmit NDP Solicitation message.
  NetNDP_TxNeighborSolicitation(if_nbr,
                                DEF_NULL,
                                (NET_IPv6_ADDR *)&p_cache_addr_ndp->AddrProtocol[0],
                                NET_NDP_NEIGHBOR_SOL_TYPE_DAD,
                                &local_err);
  if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
    status = NET_IPv6_ADDR_CFG_STATUS_FAIL;
    goto exit_update;
  }

  p_cache->ReqAttemptsCtr++;                                    // Inc req attempts ctr.

  goto exit;

exit_update:
  //                                                               ------------ RECOVER IPv6 ADDRS OBJECT -------------
  p_addr_obj = NetIPv6_GetAddrsHostOnIF(if_nbr, p_addr);
  if (p_addr_obj == DEF_NULL) {
    goto exit_clear;
  }
  //                                                               --------------- UPDATE ADDRESS STATE ---------------
  if (status == NET_IPv6_ADDR_CFG_STATUS_SUCCEED) {
    p_addr_obj->AddrState = NET_IPv6_ADDR_STATE_PREFERRED;
    p_addr_obj->IsValid = DEF_YES;
  } else {
    p_addr_obj->AddrState = NET_IPv6_ADDR_STATE_DUPLICATED;
    p_addr_obj->IsValid = DEF_NO;
  }

exit_clear:

  NetNDP_NeighborCacheRemoveEntry(p_cache, DEF_YES);            // Free NDP cache.

  NetDAD_Signal(NET_DAD_SIGNAL_TYPE_COMPL,                      // Signal that DAD process is complete.
                p_dad_obj);

  if (p_dad_obj->Fnct != DEF_NULL) {
    p_dad_obj->Fnct(if_nbr, p_dad_obj, status);
  }

exit:
  return;
}
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // NET_NDP_MODULE_EN
#endif // RTOS_MODULE_NET_AVAIL
