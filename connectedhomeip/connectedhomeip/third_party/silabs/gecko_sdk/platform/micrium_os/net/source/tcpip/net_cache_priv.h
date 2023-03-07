/***************************************************************************//**
 * @file
 * @brief Network Address Cache Management
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

#ifndef  _NET_CACHE_PRIV_H_
#define  _NET_CACHE_PRIV_H_

#include  "../../include/net_cfg_net.h"

#ifdef   NET_CACHE_MODULE_EN

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  "../../include/net_if.h"

#include  "net_def_priv.h"
#include  "net_tmr_priv.h"
#include  "net_type_priv.h"
#include  "net_stat_priv.h"

#include  <cpu/include/cpu.h>
#include  <common/source/kal/kal_priv.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                   NETWORK CACHE LIST INDEX DEFINES
 *******************************************************************************************************/

#define  NET_CACHE_ADDR_LIST_IX_ARP                        0u
#define  NET_CACHE_ADDR_LIST_IX_NDP                        1u

#define  NET_CACHE_ADDR_LIST_IX_MAX                        2u

/********************************************************************************************************
 *                                       NETWORK CACHE FLAG DEFINES
 *******************************************************************************************************/

//                                                                 ----------------- NET CACHE FLAGS ------------------
#define  NET_CACHE_FLAG_NONE                      DEF_BIT_NONE
#define  NET_CACHE_FLAG_USED                      DEF_BIT_00    // Cache cur used; i.e. NOT in free cache pool.

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                           NETWORK CACHE TYPE
 *******************************************************************************************************/

typedef  enum  net_cache_type {
  NET_CACHE_TYPE_NONE = 0,
  NET_CACHE_TYPE_ARP,
  NET_CACHE_TYPE_NDP
} NET_CACHE_TYPE;

typedef  enum net_cache_state {
  NET_CACHE_STATE_NONE,
  NET_CACHE_STATE_PEND,
  NET_CACHE_STATE_RESOLVED
} NET_CACHE_STATE;

/********************************************************************************************************
 *                                   NETWORK CACHE ADDRESS TYPE DATA TYPE
 *******************************************************************************************************/

typedef  CPU_INT16U NET_CACHE_ADDR_TYPE;

/********************************************************************************************************
 *                                   NETWORK CACHE ADDRESS DATA TYPES
 *******************************************************************************************************/
#if 0 // TODO_NET : For future use
struct  net_cache_addr {
  SLIST_MEMBER       ListNode;
  NET_CACHE_TYPE     Type;
  NET_IF_NBR         IF_Nbr;
  CPU_INT16U         AccessedCtr;
  CPU_INT16U         Flags;
  NET_BUF            *TxQ_Head;

  NET_ADDR_HW_TYPE   AddrHW_Type;
  void               *AddrHW_Ptr;
  CPU_BOOLEAN        AddrHW_Valid;

  NET_IP_ADDR_FAMILY AddrNetType;
  void               *AddrNetPtr;
  CPU_BOOLEAN        AddrNetValid;

  NET_TMR            *TmrPtr;
  CPU_INT08U         ReqAttemptsCtr;
  CPU_INT08U         State;
  CPU_INT16U         Flags;
} NET_CACHE_ADDR;

#else
//                                                                 --------------- NET CACHE ADDR -------------
typedef  struct  net_cache_addr NET_CACHE_ADDR;

struct  net_cache_addr {
  NET_CACHE_TYPE      Type;                                             // Type cfg'd @ init.

  NET_CACHE_ADDR      *PrevPtr;                                         // Ptr to PREV   addr cache.
  NET_CACHE_ADDR      *NextPtr;                                         // Ptr to NEXT   addr cache.
  void                *ParentPtr;                                       // Ptr to parent addr cache.

  NET_BUF             *TxQ_Head;                                        // Ptr to head of cache's buf Q.
  NET_BUF             *TxQ_Tail;                                        // Ptr to tail of cache's buf Q.
  NET_BUF_QTY         TxQ_Nbr;

  NET_IF_NBR          IF_Nbr;                                           // IF nbr of     addr cache.

  CPU_INT16U          AccessedCtr;                                      // Nbr successful srchs.

  CPU_INT16U          Flags;                                            // Cache flags.

  NET_CACHE_ADDR_TYPE AddrHW_Type;                                      // Remote hw       type     (see Note #2).
  CPU_INT08U          AddrHW_Len;                                       // Remote hw       addr len (see Note #2).
  CPU_BOOLEAN         AddrHW_Valid;                                     // Remote hw       addr        valid flag.

  NET_CACHE_ADDR_TYPE AddrProtocolType;                                 // Remote protocol type     (see Note #2).
  CPU_INT08U          AddrProtocolLen;                                  // Remote protocol addr len (see Note #2).
  CPU_BOOLEAN         AddrProtocolValid;                                // Remote protocol addr        valid flag.

  CPU_BOOLEAN         AddrProtocolSenderValid;                          // Remote protocol addr sender valid flag.
};

#ifdef  NET_ARP_MODULE_EN
//                                                                 ------------- NET CACHE ADDR ARP -----------
typedef  struct  net_cache_addr_arp NET_CACHE_ADDR_ARP;

struct  net_cache_addr_arp {
  NET_CACHE_TYPE      Type;                                             // Type cfg'd @ init : NET_ARP_TYPE_CACHE.

  NET_CACHE_ADDR_ARP  *PrevPtr;                                         // Ptr to PREV       ARP addr  cache.
  NET_CACHE_ADDR_ARP  *NextPtr;                                         // Ptr to NEXT       ARP addr  cache.
  void                *ParentPtr;                                       // Ptr to the parent ARP       cache.

  NET_BUF             *TxQ_Head;                                        // Ptr to head of cache's buf Q.
  NET_BUF             *TxQ_Tail;                                        // Ptr to tail of cache's buf Q.
  NET_BUF_QTY         TxQ_Nbr;

  NET_IF_NBR          IF_Nbr;                                           // IF nbr of         ARP addr  cache.

  CPU_INT16U          AccessedCtr;                                      // Nbr successful srchs.

  CPU_INT16U          Flags;                                            // Cache flags.

  NET_CACHE_ADDR_TYPE AddrHW_Type;                                      // Remote hw       type     (see Note #2).
  CPU_INT08U          AddrHW_Len;                                       // Remote hw       addr len (see Note #2).
  CPU_BOOLEAN         AddrHW_Valid;                                     // Remote hw       addr        valid flag.

  NET_CACHE_ADDR_TYPE AddrProtocolType;                                 // Remote protocol type     (see Note #2).
  CPU_INT08U          AddrProtocolLen;                                  // Remote protocol addr len (see Note #2).
  CPU_BOOLEAN         AddrProtocolValid;                                // Remote protocol addr        valid flag.

  CPU_BOOLEAN         AddrProtocolSenderValid;                          // Remote protocol addr sender valid flag.

  CPU_INT08U          AddrHW[NET_IF_HW_ADDR_LEN_MAX];                   // Remote hw       addr.

  //                                                               Remote protocol addr.
  CPU_INT08U          AddrProtocol[NET_IPv4_ADDR_SIZE];

  //                                                               Sender protocol addr.
  CPU_INT08U          AddrProtocolSender[NET_IPv4_ADDR_SIZE];
};
#endif

#ifdef  NET_NDP_MODULE_EN
//                                                                 ------------- NET CACHE ADDR NDP -----------
typedef  struct  net_cache_addr_ndp NET_CACHE_ADDR_NDP;

struct  net_cache_addr_ndp {
  NET_CACHE_TYPE      Type;                                             // Type cfg'd @ init : NET_NDP_TYPE_CACHE.

  NET_CACHE_ADDR_NDP  *PrevPtr;                                         // Ptr to PREV       NDP addr  cache.
  NET_CACHE_ADDR_NDP  *NextPtr;                                         // Ptr to NEXT       NDP addr  cache.
  void                *ParentPtr;                                       // Ptr to the parent NDP       cache.

  NET_BUF             *TxQ_Head;                                        // Ptr to head of cache's buf Q.
  NET_BUF             *TxQ_Tail;                                        // Ptr to tail of cache's buf Q.
  NET_BUF_QTY         TxQ_Nbr;

  NET_IF_NBR          IF_Nbr;                                           // IF nbr of         NDP addr  cache.

  CPU_INT16U          AccessedCtr;                                      // Nbr successful srchs.

  CPU_INT16U          Flags;                                            // Cache flags.

  NET_CACHE_ADDR_TYPE AddrHW_Type;                                      // Remote hw       type     (see Note #2).
  CPU_INT08U          AddrHW_Len;                                       // Remote hw       addr len (see Note #2).
  CPU_BOOLEAN         AddrHW_Valid;                                     // Remote hw       addr        valid flag.

  NET_CACHE_ADDR_TYPE AddrProtocolType;                                 // Remote protocol type     (see Note #2).
  CPU_INT08U          AddrProtocolLen;                                  // Remote protocol addr len (see Note #2).
  CPU_BOOLEAN         AddrProtocolValid;                                // Remote protocol addr        valid flag.

  CPU_BOOLEAN         AddrProtocolSenderValid;                          // Remote protocol addr sender valid flag.

  CPU_INT08U          AddrHW[NET_IF_HW_ADDR_LEN_MAX];                   // Remote hw       addr.

  //                                                               Remote protocol addr.
  CPU_INT08U          AddrProtocol[NET_IPv6_ADDR_SIZE];

  //                                                               Sender protocol addr.
  CPU_INT08U          AddrProtocolSender[NET_IPv6_ADDR_SIZE];
};
#endif
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

#ifdef  NET_ARP_MODULE_EN
extern NET_CACHE_ADDR_ARP NetCache_AddrARP_Tbl[NET_ARP_CFG_CACHE_NBR];
extern NET_CACHE_ADDR_ARP *NetCache_AddrARP_PoolPtr;             // Ptr to pool of free ARP caches.
#if (NET_STAT_POOL_ARP_EN == DEF_ENABLED)
extern NET_STAT_POOL NetCache_AddrARP_PoolStat;
#endif
#endif

#ifdef  NET_NDP_MODULE_EN
extern NET_CACHE_ADDR_NDP NetCache_AddrNDP_Tbl[NET_NDP_CFG_CACHE_NBR];
extern NET_CACHE_ADDR_NDP *NetCache_AddrNDP_PoolPtr;             // Ptr to pool of free NDP caches.
#if (NET_STAT_POOL_NDP_EN == DEF_ENABLED)
extern NET_STAT_POOL NetCache_AddrNDP_PoolStat;
#endif
#endif

extern NET_CACHE_ADDR *NetCache_AddrListHead[NET_CACHE_ADDR_LIST_IX_MAX];
extern NET_CACHE_ADDR *NetCache_AddrListTail[NET_CACHE_ADDR_LIST_IX_MAX];

/********************************************************************************************************
 ********************************************************************************************************
 *                                           FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

void NetCache_Init(NET_CACHE_ADDR *p_cache_parent,
                   NET_CACHE_ADDR *p_cache_addr,
                   RTOS_ERR       *p_err);

//                                                                 -------------- CFG FNCTS ---------------
CPU_BOOLEAN NetCache_CfgAccessedTh(NET_CACHE_TYPE cache_type,
                                   CPU_INT16U     nbr_access);

NET_CACHE_ADDR *NetCache_CfgAddrs(NET_CACHE_TYPE     cache_type,
                                  NET_IF_NBR         if_nbr,
                                  CPU_INT08U         *p_addr_hw,
                                  NET_CACHE_ADDR_LEN addr_hw_len,
                                  CPU_INT08U         *p_addr_protocol,
                                  CPU_INT08U         *p_addr_protocol_sender,
                                  NET_CACHE_ADDR_LEN addr_protocol_len,
                                  CPU_BOOLEAN        timer_en,
                                  CPU_FNCT_PTR       timeout_fnct,
                                  CPU_INT32U         timeout_ms,
                                  RTOS_ERR           *p_err);

//                                                                 -------------- MGMT FNCTS --------------
NET_CACHE_ADDR *NetCache_AddrSrch(NET_CACHE_TYPE     cache_type,
                                  NET_IF_NBR         if_nbr,
                                  CPU_INT08U         *p_addr_protocol,
                                  NET_CACHE_ADDR_LEN addr_protocol_len);

void NetCache_AddResolved(NET_IF_NBR     if_nbr,
                          CPU_INT08U     *p_addr_hw,
                          CPU_INT08U     *p_addr_protocol,
                          NET_CACHE_TYPE cache_type,
                          CPU_FNCT_PTR   fnct,
                          CPU_INT32U     timeout_ms,
                          RTOS_ERR       *p_err);

void NetCache_Insert(NET_CACHE_ADDR *p_cache);

void NetCache_Remove(NET_CACHE_ADDR *p_cache,
                     CPU_BOOLEAN    tmr_free);

void NetCache_UnlinkBuf(NET_BUF *p_buf);

//                                                                 --------------- TX FNCTS ---------------
void NetCache_TxPktHandler(NET_PROTOCOL_TYPE proto_type,
                           NET_BUF           *p_buf_q,
                           CPU_INT08U        *p_addr_hw);

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // NET_CACHE_MODULE_EN
#endif // _NET_CACHE_PRIV_H_
