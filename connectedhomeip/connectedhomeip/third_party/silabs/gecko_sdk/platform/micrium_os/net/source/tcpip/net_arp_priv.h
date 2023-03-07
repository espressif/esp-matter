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
 *                                               MODULE
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef  _NET_ARP_PRIV_H_
#define  _NET_ARP_PRIV_H_

#include  "../../include/net_cfg_net.h"

#ifdef   NET_ARP_MODULE_EN

/********************************************************************************************************
 ********************************************************************************************************
 *                                            INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  "net_cache_priv.h"
#include  "net_tmr_priv.h"
#include  "net_buf_priv.h"
#include  "net_stat_priv.h"

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                          ARP CACHE DEFINES
 *
 * Note(s) : (1) (a) RFC #1122, Section 2.3.2.2 states that "the link layer SHOULD save (rather than
 *                   discard) at least one ... packet of each set of packets destined to the same
 *                   unresolved IP address, and transmit the saved packet when the address has been
 *                   resolved."
 *
 *               (b) However, in order to avoid excessive discards, it seems reasonable that at least
 *                   two transmit packet buffers should be queued to a pending ARP cache.
 *******************************************************************************************************/

#define  NET_ARP_REQ_RETRY_DFLT                            3
#define  NET_ARP_REQ_RETRY_TIMEOUT_DFLT_SEC                5    // ARP req retry timeout dflt =  5 sec

#define  NET_ARP_CACHE_ACCESSED_TH_MIN                    10
#define  NET_ARP_CACHE_ACCESSED_TH_MAX                 65000
#define  NET_ARP_CACHE_ACCESSED_TH_DFLT                  100

/********************************************************************************************************
 ********************************************************************************************************
 *                                             DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                    ARP CACHE QUANTITY DATA TYPE
 *
 * Note(s) : (1) NET_ARP_CACHE_NBR_MAX  SHOULD be #define'd based on 'NET_ARP_CACHE_QTY' data type declared.
 *******************************************************************************************************/

typedef  CPU_INT16U NET_ARP_CACHE_QTY;                          // Defines max qty of ARP caches to support.

#define  NET_ARP_CACHE_NBR_MIN                             1
#define  NET_ARP_CACHE_NBR_MAX           DEF_INT_16U_MAX_VAL    // See Note #1.

/********************************************************************************************************
 *                                          ARP CACHE STATES
 *
 *                           (1a)                            (1b)
 *                               ARP REQUEST                     ARP REPLY
 *               ----------    FETCHES NEW     -----------     RESOLVES       ------------
 *               |        |     ARP CACHE      |         |     ARP CACHE      |          |
 *               |  FREE  | -----------------> | PENDING | -----------------> | RESOLVED |
 *               |        |                    |         |                    |          |
 *               ----------  (1c)              -----------                    ------------
 *                   ^  ^       ARP REQUEST          |                               |
 *                   |  |        TIMES OUT           |                               |
 *                   |  |      AFTER RETRIES         |        (1d)                   |
 *                   |  ------------------------------          ARP CACHE            |
 *                   |                                          TIMES OUT            |
 *                   -----------------------------------------------------------------
 *
 * Note(s) : (1) (a) ARP cache lookup fails to find the ARP cache with corresponding protocol address.
 *                   A new ARP cache is allocated from the ARP cache pool & inserted into the ARP Cache
 *                   List in the 'PENDING' state.  An ARP Request is generated & transmitted to resolve
 *                   the pending ARP cache.
 *
 *               (b) An ARP Reply resolves the pending ARP cache's hardware address.
 *
 *               (c) Alternatively, no corresponding ARP Reply is received after the maximum number of
 *                   ARP Request retries & the ARP cache is freed.
 *
 *               (d) ARP cache times out & is freed.
 *******************************************************************************************************/

typedef  enum  net_app_cache_state {
  NET_ARP_CACHE_STATE_NONE,
  NET_ARP_CACHE_STATE_FREE,
  NET_ARP_CACHE_STATE_PEND,
  NET_ARP_CACHE_STATE_RENEW,
  NET_ARP_CACHE_STATE_RESOLVED
} NET_ARP_CACHE_STATE;

/********************************************************************************************************
 *                                      ARP CACHE ENTRY DATA TYPE
 *
 *                                    NET_ARP_CACHE
 *                                   |-------------|
 *                                   | Cache Type  |
 *                        Previous   |-------------|
 *                         Cache <----------O      |
 *                                   |-------------|     Next
 *                                   |      O----------> Cache      Buffer Queue
 *                                   |-------------|                    Head       -------
 *                                   |      O------------------------------------> |     |
 *                                   |-------------|                               |     |
 *                                   |      O----------------------                -------
 *                                   |-------------|              |                  | ^
 *                                   |      O----------> Cache    |                  v |
 *                                   |-------------|     Timer    |                -------
 *                                   |  Hardware:  |              |                |     |
 *                                   |    Type     |              |                |     |
 *                                   |   Length    |              |                -------
 *                                   |   Address   |              |                  | ^
 *                                   |-------------|              | Buffer Queue     v |
 *                                   |  Protocol:  |              |     Tail       -------
 *                                   |    Type     |              ---------------> |     |
 *                                   |   Length    |                               |     |
 *                                   |   Address   |                               -------
 *                                   |-------------|
 *                                   |    Flags    |
 *                                   |-------------|
 *                                   |   Accessed  |
 *                                   |   Counter   |
 *                                   |-------------|
 *                                   |   Request   |
 *                                   |   Counter   |
 *                                   |-------------|
 *                                   |    State    |
 *                                   |-------------|
 *
 * Note(s) : (1) Configured by the developer via 'net_cfg.h' & 'net_cfg_net.h' at compile time.
 *
 *               See 'net_arp.h  Note #2' & 'ARP HARDWARE & PROTOCOL DEFINES  Note #1' for supported
 *               hardware & protocol types.
 *******************************************************************************************************/

//                                                                 --------------- NET ARP CACHE --------------
typedef  struct  net_arp_cache NET_ARP_CACHE;

struct  net_arp_cache {
  NET_CACHE_TYPE      Type;
  NET_CACHE_ADDR_ARP  *CacheAddrPtr;                                    // Ptr to ARP addr cache.
  NET_TMR             *TmrPtr;                                          // Ptr to cache TMR.
  CPU_INT08U          ReqAttemptsCtr;                                   // ARP req attempts ctr.
  NET_ARP_CACHE_STATE State;                                            // ARP cache state.
  CPU_INT16U          Flags;                                            // ARP cache flags.
};

/********************************************************************************************************
 ********************************************************************************************************
 *                                          GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                         FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

#ifdef NET_IPv4_MODULE_EN
void NetARP_Init(RTOS_ERR *p_err);

NET_CACHE_STATE NetARP_CacheHandler(NET_BUF  *p_buf,
                                    RTOS_ERR *p_err);

void NetARP_Rx(NET_BUF  *p_buf,
               RTOS_ERR *p_err);

void NetARP_TxReq(NET_CACHE_ADDR_ARP *p_cache);

void NetARP_CacheAddPend(NET_BUF     *p_buf,
                         NET_BUF_HDR *p_buf_hdr,
                         CPU_INT08U  *p_addr_protocol,
                         RTOS_ERR    *p_err);

void NetARP_CacheRemoveEntry(NET_ARP_CACHE *p_cache,
                             CPU_BOOLEAN   tmr_free);
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                        CONFIGURATION ERRORS
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef  NET_ARP_CFG_CACHE_NBR
#error  "NET_ARP_CFG_CACHE_NBR                not #define'd in 'net_cfg.h' "
#error  "                               [MUST be  >= NET_ARP_CACHE_NBR_MIN]"
#error  "                               [     &&  <= NET_ARP_CACHE_NBR_MAX]"

#elif  ((NET_ARP_CFG_CACHE_NBR < NET_ARP_CACHE_NBR_MIN) \
  || (NET_ARP_CFG_CACHE_NBR > NET_ARP_CACHE_NBR_MAX))
#error  "NET_ARP_CFG_NBR_CACHE          illegally #define'd in 'net_cfg.h' "
#error  "                               [MUST be  >= NET_ARP_CACHE_NBR_MIN]"
#error  "                               [     &&  <= NET_ARP_CACHE_NBR_MAX]"
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                             MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/
#endif // NET_ARP_MODULE_EN
#endif // _NET_ARP_PRIV_H_
