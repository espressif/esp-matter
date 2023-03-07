/***************************************************************************//**
 * @file
 * @brief Network Counter Management
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

#ifndef  _NET_CTR_PRIV_H_
#define  _NET_CTR_PRIV_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <em_core.h>

#include  "../../include/net_cfg_net.h"
#include  "../../include/net_type.h"
#include  "net_def_priv.h"

#include  <common/source/rtos/rtos_utils_priv.h>
#include  <cpu/include/cpu.h>
#include  <common/include/rtos_path.h>
#include  <net_cfg.h>
#include  <rtos_cfg.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                           CONFIGURATION ERRORS
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef  NET_CTR_CFG_STAT_EN
#error  "NET_CTR_CFG_STAT_EN        not #define'd in 'net_cfg.h'"
#error  "                     [MUST be  DEF_DISABLED]           "
#error  "                     [     ||  DEF_ENABLED ]           "

#elif  ((NET_CTR_CFG_STAT_EN != DEF_DISABLED) \
  && (NET_CTR_CFG_STAT_EN != DEF_ENABLED))
#error  "NET_CTR_CFG_STAT_EN  illegally #define'd in 'net_cfg.h'"
#error  "                     [MUST be  DEF_DISABLED]           "
#error  "                     [     ||  DEF_ENABLED ]           "
#endif

#ifndef  NET_CTR_CFG_ERR_EN
#error  "NET_CTR_CFG_ERR_EN         not #define'd in 'net_cfg.h'"
#error  "                     [MUST be  DEF_DISABLED]           "
#error  "                     [     ||  DEF_ENABLED ]           "

#elif  ((NET_CTR_CFG_ERR_EN != DEF_DISABLED) \
  && (NET_CTR_CFG_ERR_EN != DEF_ENABLED))
#error  "NET_CTR_CFG_ERR_EN   illegally #define'd in 'net_cfg.h'"
#error  "                     [MUST be  DEF_DISABLED]           "
#error  "                     [     ||  DEF_ENABLED ]           "
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                       NETWORK COUNTER DATA TYPE
 *
 * Note(s) : (1) NET_CTR_MIN & NET_CTR_MAX  SHOULD be #define'd based on 'NET_CTR' data type declared.
 *
 *           (2) NET_CTR_PCT_NUMER_MAX_TH, NET_CTR_BIT_LO, & NET_CTR_BIT_HI  MUST be globally #define'd
 *               AFTER 'NET_CTR' data type declared.
 *
 *           (3) NET_CTR_PCT_NUMER_MAX_TH  #define's the maximum value for a counter that is used as a
 *               numerator in a percentage calculation.  This threshold is required since the numerator in
 *               a percentage calculation is multiplied by 100 (%) BEFORE division with the denominator :
 *
 *                                       Numerator * 100%
 *                   Percentage (%) = ----------------
 *                                       Denominator
 *
 *               Therefore, the numerator MUST be constrained by this threshold to prevent integer overflow
 *               from the multiplication BEFORE the division.
 *
 *               (a) The percentage threshold value is calculated as follows :
 *
 *                                                   N
 *                                               2
 *                       Percentage Threshold = ---
 *                                               100
 *
 *               (b) To avoid macro integer overflow, the threshold value is modified by one less "divide-by-2"
 *                   left-shift compensated by dividing the numerator by 50, instead of 100 :
 *
 *                                               N-1     N
 *                                               2       2  / 2
 *                       Percentage Threshold = ---- = -------
 *                                               50    100 / 2
 *******************************************************************************************************/

#define  NET_CTR_MIN                    DEF_INT_32U_MIN_VAL     // Define as min unsigned val (see Note #1).
#define  NET_CTR_MAX                    DEF_INT_32U_MAX_VAL     // Define as max unsigned val (see Note #1).

#define  NET_CTR_PCT_NUMER_MAX_TH     ((NET_CTR)(((NET_CTR)1u << ((sizeof(NET_CTR) * DEF_OCTET_NBR_BITS) - 1u)) / (NET_CTR)50u))

#define  NET_CTR_BIT_LO                          ((NET_CTR)DEF_BIT_00)
#define  NET_CTR_BIT_HI               ((NET_CTR) ((NET_CTR)1u << ((sizeof(NET_CTR) * DEF_OCTET_NBR_BITS) - 1u)))

/********************************************************************************************************
 *                                       NETWORK STATISTIC DATA TYPES
 *******************************************************************************************************/

/*--------------------------------------------------------------------------------------------------------
 *                                   INTERFACE 802x STATISTIC COUNTERS
 *------------------------------------------------------------------------------------------------------*/

typedef  struct  net_ctr_if_802x_stats {
  NET_CTR RxCtr;
  NET_CTR RxPktCtr;                                         // Nbr rx'd 802x  pkts.
  NET_CTR RxPktDisCtr;                                      // Nbr rx'd 802x  pkts discarded by the interface.

  NET_CTR TxPktCtr;                                         // Nbr tx'd 802x  pkts.

  NET_CTR RxPktBcastCtr;                                    // Nbr rx'd 802x  pkts broadcast to this     dest.
  NET_CTR TxPktBcastCtr;                                    // Nbr tx'd 802x  pkts broadcast to multiple dest(s).

    #ifdef  NET_MCAST_RX_MODULE_EN
  NET_CTR RxPktMcastCtr;                                    // Nbr rx'd 802x  pkts multicast to this     dest.
    #endif

    #ifdef  NET_MCAST_TX_MODULE_EN
  NET_CTR TxPktMcastCtr;                                    // Nbr tx'd 802x  pkts multicast to multiple dest(s).
    #endif
} NET_CTR_IF_802x_STATS;

/*--------------------------------------------------------------------------------------------------------
 *                                   INTERFACE GENERIC STATISTIC COUNTERS
 *------------------------------------------------------------------------------------------------------*/

typedef  struct  net_ctr_if_stats {
  NET_CTR RxNbrOctets;                                      // Nbr      octets      rx'd      for a specific IF.
  NET_CTR RxNbrOctetsPrev;                                  // Nbr      octets prev rx'd      for a specific IF.
  NET_CTR RxNbrOctetsPerSec;                                // Nbr rx'd octets per  sec       for a specific IF.
  NET_CTR RxNbrOctetsPerSecMax;                             // Nbr rx'd octets per  sec max   for a specific IF.
  NET_CTR RxNbrPktCtr;                                      // Nbr rx'd pkts                  for a specific IF.
  NET_CTR RxNbrPktCtrPerSec;                                // Nbr rx'd pkts   per  sec       for a specific IF.
  NET_CTR RxNbrPktCtrPerSecMax;                             // Nbr rx'd pkts   per  sec max   for a specific IF.
  NET_CTR RxNbrPktCtrProcessed;                             // Nbr rx'd pkts        processed for a specific IF.
  NET_CTR RxNbrPktCtrProcessedPrev;                         // Nbr rx'd pkts   prev processed for a specific IF.

  NET_CTR TxNbrOctets;                                      // Nbr      octets      tx'd      for a specific IF.
  NET_CTR TxNbrOctetsPrev;                                  // Nbr      octets prev tx'd      for a specific IF.
  NET_CTR TxNbrOctetsPerSec;                                // Nbr tx'd octets per  sec       for a specific IF.
  NET_CTR TxNbrOctetsPerSecMax;                             // Nbr tx'd octets per  sec max   for a specific IF.
  NET_CTR TxNbrPktCtr;                                      // Nbr tx'd pkts                  for a specific IF.
  NET_CTR TxNbrPktCtrPerSec;                                // Nbr tx'd pkts   per  sec       for a specific IF.
  NET_CTR TxNbrPktCtrPerSecMax;                             // Nbr tx'd pkts   per  sec max   for a specific IF.
  NET_CTR TxNbrPktCtrProcessed;                             // Nbr tx'd pkts        processed for a specific IF.
  NET_CTR TxNbrPktCtrProcessedPrev;                         // Nbr tx'd pkts   prev processed for a specific IF.
} NET_CTR_IF_STATS;

/*--------------------------------------------------------------------------------------------------------
 *                                   LOOPBACK INTERFACE STATISTIC COUNTERS
 *------------------------------------------------------------------------------------------------------*/

typedef  struct  net_ctr_if_loopback_stats {
  NET_CTR RxPktCtr;                                         // Nbr rx'd loopback pkts.
  NET_CTR RxPktCompCtr;                                     // Nbr rx'd loopback pkts delivered to supported protocols.

  NET_CTR TxPktCtr;                                         // Nbr tx'd loopback pkts.
} NET_CTR_IF_LOOPBACK_STATS;

/*--------------------------------------------------------------------------------------------------------
 *                                   ETHERNET INTERFACE STATISTIC COUNTERS
 *------------------------------------------------------------------------------------------------------*/

typedef  struct  net_ctr_if_ether_stats {
  NET_CTR_IF_802x_STATS IF_802xCtrs;                        // Ether 802x stats ctrs.

  NET_CTR               RxPktCtr;                           // Nbr rx'd ether pkts.
  NET_CTR               TxPktCtr;                           // Nbr tx'd ether pkts.
} NET_CTR_IF_ETHER_STATS;

/*--------------------------------------------------------------------------------------------------------
 *                                   WIFI INTERFACE STATISTIC COUNTERS
 *------------------------------------------------------------------------------------------------------*/

typedef  struct  net_ctr_if_wifi_stats {
  NET_CTR_IF_802x_STATS IF_802xCtrs;                        // Wifi 802x stats ctrs.

  NET_CTR               RxPktCtr;                           // Nbr rx'd wifi pkts.
  NET_CTR               RxMgmtCtr;                          // Nbr rx'd wifi mgmt frame.
  NET_CTR               RxMgmtCompCtr;                      // Nbr rx'd wifi mgmt frame delivered to wifi mgr.
  NET_CTR               TxPktCtr;                           // Nbr tx'd wifi pkts.
} NET_CTR_IF_WIFI_STATS;

/*--------------------------------------------------------------------------------------------------------
 *                                       INTERFACES STATISTIC COUNTERS
 *------------------------------------------------------------------------------------------------------*/

typedef  struct  net_ctr_ifs_stats {
  NET_CTR_IF_STATS          *IF;                            // IF stat ctrs.

  NET_CTR                   RxPktCtr;                       // Nbr rx'd IF pkts.

  NET_CTR                   TxPktCtr;                       // Nbr tx'd IF pkts.
  NET_CTR                   TxPktDeallocCtr;                // Nbr tx'd IF pkts successfully dealloc'd.

    #ifdef  NET_IF_LOOPBACK_MODULE_EN
  NET_CTR_IF_LOOPBACK_STATS Loopback;                       // Loopback interfaces statistics.
    #endif

    #ifdef  NET_IF_ETHER_MODULE_EN
  NET_CTR_IF_ETHER_STATS Ether;                             // Ethernet interfaces statistics.
    #endif

    #ifdef  NET_IF_WIFI_MODULE_EN
  NET_CTR_IF_WIFI_STATS WiFi;                               // WiFi interfaces statistics.
    #endif

    #ifdef  NET_IF_802x_MODULE_EN
  NET_CTR_IF_802x_STATS IFs_802xCtrs;                       // 802x interface statistics.
    #endif
} NET_CTR_IFs_STATS;

/*--------------------------------------------------------------------------------------------------------
 *                                       ARP STATISTIC COUNTERS
 *------------------------------------------------------------------------------------------------------*/

typedef  struct  net_ctr_arp_stats {
  NET_CTR RxPktCtr;                                         // Nbr rx'd ARP       pkts.
  NET_CTR RxMsgCompCtr;                                     // Nbr rx'd ARP       msgs successfully processed.
  NET_CTR RxMsgReqCompCtr;                                  // Nbr rx'd ARP req   msgs successfully processed.
  NET_CTR RxMsgReplyCompCtr;                                // Nbr rx'd ARP reply msgs successfully processed.

  NET_CTR TxMsgCtr;                                         // Nbr tx'd ARP       msgs.
  NET_CTR TxMsgReqCtr;                                      // Nbr tx'd ARP req   msgs.
  NET_CTR TxMsgReplyCtr;                                    // Nbr tx'd ARP reply msgs.
} NET_CTR_ARP_STATS;

/*--------------------------------------------------------------------------------------------------------
 *                                       NDP STATISTIC COUNTERS
 *------------------------------------------------------------------------------------------------------*/

typedef  struct  net_ctr_ndp_stats {
  NET_CTR RxMsgCtr;
  NET_CTR RxMsgSolNborCtr;                                  // Nbr rx'd NDP neighbor solicitation  msgs.
  NET_CTR RxMsgAdvNborCtr;                                  // Nbr rx'd NDP neighbor advertisement msgs.
  NET_CTR RxMsgAdvRouterCtr;                                // Nbr rx'd NDP router   advertisement msgs.
  NET_CTR RxMsgRedirectCtr;                                 // Nbr rx'd NDP redirect               msgs.
} NET_CTR_NDP_STATS;

/*--------------------------------------------------------------------------------------------------------
 *                                       IPv4 STATISTIC COUNTERS
 *------------------------------------------------------------------------------------------------------*/

typedef  struct  net_ctr_ipv4_stats {
  NET_CTR RxPktCtr;                                         // Nbr rx'd IPv4 datagrams.
  NET_CTR RxDgramCompCtr;                                   // Nbr rx'd IPv4 datagrams delivered to supported protocols.

  NET_CTR RxDestLocalHostCtr;                               // Nbr rx'd IPv4 datagrams from localhost.
  NET_CTR RxDestBcastCtr;                                   // Nbr rx'd IPv4 datagrams via  broadcast.

    #ifdef  NET_IGMP_MODULE_EN
  NET_CTR RxDestMcastCtr;                                   // Nbr rx'd IPv4 datagrams via  multicast.
    #endif

  NET_CTR RxFragCtr;                                        // Nbr rx'd IPv4 frags.
  NET_CTR RxFragDgramReasmCtr;                              // Nbr rx'd IPv4 frag'd datagrams reasm'd.

  NET_CTR TxDgramCtr;                                       // Nbr tx'd IPv4 datagrams.
  NET_CTR TxDestThisHostCtr;                                // Nbr tx'd IPv4 datagrams           to this host.
  NET_CTR TxDestLocalHostCtr;                               // Nbr tx'd IPv4 datagrams           to localhost.
  NET_CTR TxDestLocalLinkCtr;                               // Nbr tx'd IPv4 datagrams           to local  link addr(s).
  NET_CTR TxDestLocalNetCtr;                                // Nbr tx'd IPv4 datagrams           to local  net.
  NET_CTR TxDestRemoteNetCtr;                               // Nbr tx'd IPv4 datagrams           to remote net.
  NET_CTR TxDestBcastCtr;                                   // Nbr tx'd IPv4 datagrams broadcast to dest(s).

    #ifdef  NET_MCAST_MODULE_EN
  NET_CTR TxDestMcastCtr;                                   // Nbr tx'd IPv4 datagrams multicast to dest(s).
    #endif
} NET_CTR_IPv4_STATS;

/*--------------------------------------------------------------------------------------------------------
 *                                       IPv6 STATISTIC COUNTERS
 *------------------------------------------------------------------------------------------------------*/

typedef  struct  net_ctr_ipv6_stats {
  NET_CTR RxPktCtr;                                         // Nbr rx'd IPv6 datagrams.
  NET_CTR RxDgramCompCtr;                                   // Nbr rx'd IPv6 datagrams delivered to supported protocols.

  NET_CTR RxDestLocalHostCtr;                               // Nbr rx'd IPv6 datagrams from localhost.
  NET_CTR RxDestBcastCtr;                                   // Nbr rx'd IPv6 datagrams via  broadcast.

  NET_CTR RxFragCtr;                                        // Nbr rx'd IPv6 frags.
  NET_CTR RxFragDgramReasmCtr;                              // Nbr rx'd IPv6 frag'd datagrams reasm'd.

  NET_CTR TxDgramCtr;                                       // Nbr tx'd IPv6 datagrams.
  NET_CTR TxDestThisHostCtr;                                // Nbr tx'd IPv6 datagrams           to this host.
  NET_CTR TxDestLocalHostCtr;                               // Nbr tx'd IPv6 datagrams           to localhost.
  NET_CTR TxDestLocalLinkCtr;                               // Nbr tx'd IPv6 datagrams           to local  link addr(s).
  NET_CTR TxDestLocalNetCtr;                                // Nbr tx'd IPv6 datagrams           to local  net.
  NET_CTR TxDestRemoteNetCtr;                               // Nbr tx'd IPv6 datagrams           to remote net.
  NET_CTR TxDestBcastCtr;                                   // Nbr tx'd IPv6 datagrams broadcast to dest(s).
} NET_CTR_IPv6_STATS;

/*--------------------------------------------------------------------------------------------------------
 *                                       ICMPv4 STATISTIC COUNTERS
 *------------------------------------------------------------------------------------------------------*/

typedef  struct  net_ctr_icmpv4_stats {
  NET_CTR RxMsgCtr;                                         // Nbr rx'd ICMPv4       msgs.
  NET_CTR RxMsgCompCtr;                                     // Nbr rx'd ICMPv4       msgs successfully processed.
  NET_CTR RxMsgErrCtr;                                      // Nbr rx'd ICMPv4 err   msgs successfully processed.
  NET_CTR RxMsgReqCtr;                                      // Nbr rx'd ICMPv4 req   msgs successfully processed.
  NET_CTR RxMsgReplyCtr;                                    // Nbr rx'd ICMPv4 reply msgs successfully processed.
  NET_CTR RxMsgUnknownCtr;

  NET_CTR TxMsgCtr;                                         // Nbr tx'd ICMPv4       msgs.
  NET_CTR TxMsgErrCtr;                                      // Nbr tx'd ICMPv4 err   msgs.
  NET_CTR TxMsgReqCtr;                                      // Nbr tx'd ICMPv4 req   msgs.
  NET_CTR TxMsgReplyCtr;                                    // Nbr tx'd ICMPv4 reply msgs.
} NET_CTR_ICMPv4_STATS;

/*--------------------------------------------------------------------------------------------------------
 *                                       ICMPv6 STATISTIC COUNTERS
 *------------------------------------------------------------------------------------------------------*/

typedef  struct  net_ctr_icmpv6_stats {
  NET_CTR RxMsgCtr;                                         // Nbr rx'd ICMPv6       msgs.
  NET_CTR RxMsgCompCtr;                                     // Nbr rx'd ICMPv6       msgs successfully processed.
  NET_CTR RxMsgErrCtr;                                      // Nbr rx'd ICMPv6 err   msgs successfully processed.
  NET_CTR RxMsgReqCtr;                                      // Nbr rx'd ICMPv6 req   msgs successfully processed.
  NET_CTR RxMsgReplyCtr;                                    // Nbr rx'd ICMPv6 reply msgs successfully processed.

  NET_CTR RxMsgUnknownCtr;

  NET_CTR TxMsgCtr;                                         // Nbr tx'd ICMPv6       msgs.
  NET_CTR TxMsgErrCtr;                                      // Nbr tx'd ICMPv6 err   msgs.
  NET_CTR TxMsgReqCtr;                                      // Nbr tx'd ICMPv6 req   msgs.
  NET_CTR TxMsgReplyCtr;                                    // Nbr tx'd ICMPv6 reply msgs.
} NET_CTR_ICMPv6_STATS;

/*--------------------------------------------------------------------------------------------------------
 *                                       IGMP STATISTIC COUNTERS
 *------------------------------------------------------------------------------------------------------*/

typedef  struct  net_ctr_igmp_stats {
  NET_CTR RxMsgCtr;                                         // Nbr rx'd IGMP        msgs.
  NET_CTR RxMsgCompCtr;                                     // Nbr rx'd IGMP        msgs successfully processed.
  NET_CTR RxMsgQueryCtr;                                    // Nbr rx'd IGMP query  msgs successfully processed.
  NET_CTR RxMsgReportCtr;                                   // Nbr rx'd IGMP report msgs successfully processed.

  NET_CTR TxMsgCtr;                                         // Nbr tx'd IGMP        msgs.
  NET_CTR TxMsgReportCtr;                                   // Nbr tx'd IGMP report msgs.
} NET_CTR_IGMP_STATS;

/*--------------------------------------------------------------------------------------------------------
 *                                       MLDP STATISTIC COUNTERS
 *------------------------------------------------------------------------------------------------------*/

typedef  struct  net_ctr_mldp_stats {
  NET_CTR RxMsgCtr;                                         // Nbr rx'd MLDP        msgs.
  NET_CTR RxMsgCompCtr;                                     // Nbr rx'd MLDP        msgs successfully processed.
  NET_CTR RxMsgQueryCtr;                                    // Nbr rx'd MLDP query  msgs successfully processed.
  NET_CTR RxMsgReportCtr;                                   // Nbr rx'd MLDP report msgs successfully processed.

  NET_CTR TxMsgCtr;                                         // Nbr tx'd MLDP        msgs.
  NET_CTR TxMsgReportCtr;                                   // Nbr tx'd MLDP report msgs.
} NET_CTR_MLDP_STATS;

/*--------------------------------------------------------------------------------------------------------
 *                                       UDP STATISTIC COUNTERS
 *------------------------------------------------------------------------------------------------------*/

typedef  struct  net_ctr_udp_stats {
  NET_CTR RxPktCtr;                                         // Nbr rx'd UDP datagrams.
  NET_CTR RxDgramCompCtr;                                   // Nbr rx'd UDP datagrams delivered to app layer.

  NET_CTR TxDgramCtr;                                       // Nbr tx'd UDP datagrams.
} NET_CTR_UDP_STATS;

/*--------------------------------------------------------------------------------------------------------
 *                                       TCP STATISTIC COUNTERS
 *------------------------------------------------------------------------------------------------------*/

typedef  struct  net_ctr_tcp_stats {
  NET_CTR RxPktCtr;                                         // Nbr rx'd TCP segs.
  NET_CTR RxSegCompCtr;                                     // Nbr rx'd TCP segs demux'd to app conn.

  NET_CTR TxSegCtr;                                         // Nbr tx'd TCP                 segs.
  NET_CTR TxSegConnSyncCtr;                                 // Nbr tx'd TCP conn sync       segs.
  NET_CTR TxSegConnCloseCtr;                                // Nbr tx'd TCP conn close      segs.
  NET_CTR TxSegConnAckCtr;                                  // Nbr tx'd TCP conn ack        segs.
  NET_CTR TxSegConnResetCtr;                                // Nbr tx'd TCP conn reset      segs.
  NET_CTR TxSegConnProbeCtr;                                // Nbr tx'd TCP conn probe      segs.
  NET_CTR TxSegConnKAliveCtr;                               // Nbr tx'd TCP conn keep-alive segs.
  NET_CTR TxSegConnTxQ_Ctr;                                 // Nbr tx'd TCP conn    tx Q    segs.
  NET_CTR TxSegConnReTxQ_Ctr;                               // Nbr tx'd TCP conn re-tx Q    segs.
} NET_CTR_TCP_STATS;

/*--------------------------------------------------------------------------------------------------------
 *                                       SOCKET STATISTIC COUNTERS
 *------------------------------------------------------------------------------------------------------*/

typedef  struct  net_ctr_sock_stats {
  NET_CTR RxPktCtr;                                         // Nbr rx'd sock pkts.
  NET_CTR RxPktCompCtr;                                     // Nbr rx'd sock pkts delivered to apps.
} NET_CTR_SOCK_STATS;

/*--------------------------------------------------------------------------------------------------------
 *                                           STATISTIC COUNTERS
 *------------------------------------------------------------------------------------------------------*/

typedef  struct  net_ctr_stats {
  NET_CTR_IFs_STATS IFs;

    #ifdef  NET_ARP_MODULE_EN
  NET_CTR_ARP_STATS ARP;
    #endif

    #ifdef  NET_NDP_MODULE_EN
  NET_CTR_NDP_STATS NDP;
    #endif

    #ifdef NET_IPv4_MODULE_EN
  NET_CTR_IPv4_STATS IPv4;
    #endif

    #ifdef  NET_IPv6_MODULE_EN
  NET_CTR_IPv6_STATS IPv6;
    #endif

    #ifdef  NET_ICMPv4_MODULE_EN
  NET_CTR_ICMPv4_STATS ICMPv4;
    #endif

    #ifdef  NET_ICMPv6_MODULE_EN
  NET_CTR_ICMPv6_STATS ICMPv6;
    #endif

    #ifdef  NET_IGMP_MODULE_EN
  NET_CTR_IGMP_STATS IGMP;
    #endif

    #ifdef  NET_MLDP_MODULE_EN
  NET_CTR_MLDP_STATS MLDP;
    #endif

  NET_CTR_UDP_STATS  UDP;

    #ifdef  NET_TCP_MODULE_EN
  NET_CTR_TCP_STATS  TCP;
    #endif

  NET_CTR_SOCK_STATS Sock;
} NET_CTR_STATS;

/********************************************************************************************************
 *                                       NETWORK ERROR DATA TYPES
 *******************************************************************************************************/

/*--------------------------------------------------------------------------------------------------------
 *                                       802x INTERFACE ERROR COUNTERS
 *------------------------------------------------------------------------------------------------------*/

typedef  struct  net_ctr_if_802x_errs {
  NET_CTR NullPtrCtr;                                       // Nbr null Ether IF ptr accesses.

  NET_CTR RxInvFrameCtr;                                    // Nbr rx'd Ether pkts with invalid frame.
  NET_CTR RxInvAddrDestCtr;                                 // Nbr rx'd Ether pkts with invalid dest addr.
  NET_CTR RxInvAddrSrcCtr;                                  // Nbr rx'd Ether pkts with invalid src  addr.
  NET_CTR RxInvProtocolCtr;                                 // Nbr rx'd Ether pkts with invalid/unsupported protocol.
  NET_CTR RxPktDisCtr;                                      // Nbr rx'd Ether pkts discarded.
  NET_CTR RxInvBufIxCtr;                                    // Nbr rx'd Ether pkts with invalid buf ix.
  NET_CTR RxIFDisableCtr;

  NET_CTR TxPktDisCtr;                                      // Nbr tx   Ether pkts discarded.
  NET_CTR TxInvProtocolCtr;                                 // Nbr tx   Ether pkts with invalid/unsupported protocol.
  NET_CTR TxInvBufIxCtr;                                    // Nbr tx   Ether pkts with invalid buf ix.
  NET_CTR TxHdrDataLenCtr;
} NET_CTR_IF_802x_ERRS;

/*--------------------------------------------------------------------------------------------------------
 *                                   INTERFACE GENERIC ERROR COUNTERS
 *------------------------------------------------------------------------------------------------------*/

typedef  struct  net_ctr_if_errs {
  NET_CTR BufLostCtr;                                       // Nbr      IF net bufs lost/discarded.

  NET_CTR RxInvProtocolCtr;                                 // Nbr rx'd IF pkts with invalid/unsupported protocol.
  NET_CTR RxInvBufIxCtr;                                    // Nbr rx'd IF pkts with invalid buf ix.
  NET_CTR RxPktDisCtr;                                      // Nbr rx'd IF pkts discarded for a specific IF.

  NET_CTR TxInvProtocolCtr;                                 // Nbr tx   IF pkts with invalid/unsupported protocol.
  NET_CTR TxInvBufIxCtr;                                    // Nbr tx   IF pkts with invalid buf ix.
  NET_CTR TxPktDisCtr;                                      // Nbr tx   IF pkts discarded for a specific IF.
} NET_CTR_IF_ERRS;

/*--------------------------------------------------------------------------------------------------------
 *                                   LOOPBACK INTERFACE ERROR COUNTERS
 *------------------------------------------------------------------------------------------------------*/

typedef  struct  net_ctr_if_loopback_errs {
  NET_CTR RxPktDisCtr;                                      // Nbr rx'd loopback pkts discarded.
  NET_CTR TxPktDisCtr;                                      // Nbr tx   loopback pkts discarded.
} NET_CTR_IF_LOOPBACK_ERRS;

/*--------------------------------------------------------------------------------------------------------
 *                                   ETHERNET INTERFACE ERROR COUNTERS
 *------------------------------------------------------------------------------------------------------*/

typedef  struct  net_ctr_if_ether_errs {
  NET_CTR_IF_802x_ERRS IF_802xCtrs;                             // Ether 802x err ctrs.

  NET_CTR              RxPktDisCtr;                             // Nbr rx'd Ether pkts discarded.

  NET_CTR              TxPktDisCtr;                             // Nbr tx   Ether pkts discarded.
} NET_CTR_IF_ETHER_ERRS;

/*--------------------------------------------------------------------------------------------------------
 *                                       WIFI INTERFACE ERROR COUNTERS
 *------------------------------------------------------------------------------------------------------*/

typedef  struct  net_ctr_if_wifi_errs {
  NET_CTR_IF_802x_ERRS IF_802xCtrs;                         // Wifi 802x err ctrs.

  NET_CTR              RxDisCtr;                            // Nbr rx'd Wifi pkts or mgmt frame discarded.
  NET_CTR              RxPktDisCtr;                         // Nbr rx'd Wifi pkts               discarded.
  NET_CTR              RxMgmtDisCtr;                        // Nbr rx'd Wifi         mgmt frame discarded.

  NET_CTR              TxPktDisCtr;                         // Nbr tx   Wifi pkts discarded.
} NET_CTR_IF_WIFI_ERRS;

/*--------------------------------------------------------------------------------------------------------
 *                                       INTERFACES ERROR COUNTERS
 *------------------------------------------------------------------------------------------------------*/

typedef  struct  net_ctr_ifs_errs {
  NET_CTR_IF_ERRS          *IF;                         // IF-Dev err ctrs.

  NET_CTR                  RxPktDisCtr;                 // Nbr rx'd IF pkts discarded.
  NET_CTR                  TxPktDeallocCtr;             // Nbr tx'd IF pkts NOT sucessfully dealloc'd.

  NET_CTR                  TxPktDisCtr;                 // Nbr tx'd IF pkts discarded.
  NET_CTR                  InvTransactionTypeCtr;       // Nbr invalid transaction type accesses.

#ifdef  NET_IF_LOOPBACK_MODULE_EN
  NET_CTR_IF_LOOPBACK_ERRS Loopback;
#endif // NET_IF_LOOPBACK_MODULE_EN

#ifdef  NET_IF_ETHER_MODULE_EN
  NET_CTR_IF_ETHER_ERRS Ether;
#endif // NET_IF_ETHER_MODULE_EN

#ifdef  NET_IF_WIFI_MODULE_EN
  NET_CTR_IF_WIFI_ERRS WiFi;
#endif // NET_IF_WIFI_MODULE_EN

#ifdef  NET_IF_802x_MODULE_EN
  NET_CTR_IF_802x_ERRS IFs_802x;                        // Sum of 802x interfaces errors.
#endif // NET_IF_802x_MODULE_EN
} NET_CTR_IFs_ERRS;

/*--------------------------------------------------------------------------------------------------------
 *                                           TIMER ERROR COUNTERS
 *------------------------------------------------------------------------------------------------------*/

typedef  struct  net_ctr_tmr_errs {
  NET_CTR NoneAvailCtr;                                 // Nbr unavail net tmr      accesses.
} NET_CTR_TMR_ERRS;

/*--------------------------------------------------------------------------------------------------------
 *                                   BUFFER MANAGEMENT ERROR COUNTERS
 *------------------------------------------------------------------------------------------------------*/

typedef  struct  net_ctr_buf_errs {
  NET_CTR NoneAvailCtr;                                     // Nbr unavail net buf      accesses.
  NET_CTR InvTypeCtr;                                       // Nbr invalid net buf type accesses.
  NET_CTR SizeCtr;                                          // Nbr         net bufs with invalid size.
  NET_CTR LenCtr;                                           // Nbr         net bufs with invalid len.
  NET_CTR InvTransactionTypeCtr;
  NET_CTR NotUsedCtr;                                       // Nbr unused  net buf      accesses.
} NET_CTR_BUF_ERRS;

/*--------------------------------------------------------------------------------------------------------
 *                                   CONNECTION MANAGEMENT ERROR COUNTERS
 *------------------------------------------------------------------------------------------------------*/

typedef  struct  net_ctr_conn_errs {
  NET_CTR NoneAvailCtr;                                     // Nbr unavail net conn      accesses.
  NET_CTR NotUsedCtr;                                       // Nbr unused  net conn      accesses.

  NET_CTR CloseCtr;                                         // Nbr         net conn closes.

  NET_CTR InvConnCtr;                                       // Nbr invalid net conn ID   accesses.
  NET_CTR InvConnAddrLenCtr;                                // Nbr         net conns with invalid addr len.
  NET_CTR InvConnAddrInUseCtr;                              // Nbr         net conns with         addr already in use.

  NET_CTR InvFamilyCtr;                                     // Nbr         net conns with invalid conn family.
  NET_CTR InvProtocolIxCtr;                                 // Nbr         net conns with invalid protocol ix.
} NET_CTR_CONN_ERRS;

/*--------------------------------------------------------------------------------------------------------
 *                                           ARP ERROR COUNTERS
 *------------------------------------------------------------------------------------------------------*/

typedef struct  net_ctr_arp_errs {
  NET_CTR NoneAvailCtr;                                     // Nbr unavail ARP cache      accesses.

  NET_CTR RxInvHW_TypeCtr;                                  // Nbr rx'd ARP msgs with invalid hw       type.
  NET_CTR RxInvHW_AddrLenCtr;                               // Nbr rx'd ARP msgs with invalid hw       addr len.
  NET_CTR RxInvHW_AddrCtr;                                  // Nbr rx'd ARP msgs with invalid hw       addr.
  NET_CTR RxInvProtocolTypeCtr;                             // Nbr rx'd ARP msgs with invalid protocol type.
  NET_CTR RxInvLenCtr;                                      // Nbr rx'd ARP msgs with invalid protocol addr len.
  NET_CTR RxInvProtocolAddrCtr;                             // Nbr rx'd ARP msgs with invalid protocol addr.
  NET_CTR RxInvOpCodeCtr;                                   // Nbr rx'd ARP msgs with invalid op code.
  NET_CTR RxInvOpAddrCtr;                                   // Nbr rx'd ARP msgs with invalid op code/addr.
  NET_CTR RxInvMsgLenCtr;                                   // Nbr rx'd ARP msgs with invalid msg len.
  NET_CTR RxPktInvDest;                                     // Nbr rx'd ARP msgs for  invalid           dest.

  NET_CTR RxPktTargetReplyCtr;                              // Nbr rx'd ARP msgs for  invalid reply msg dest.

  NET_CTR RxPktDisCtr;                                      // Nbr rx'd ARP pkts discarded.

  NET_CTR TxInvLenCtr;
  NET_CTR TxInvBufIxCtr;                                    // Nbr tx   ARP pkts with invalid buf ix.
  NET_CTR TxPktDisCtr;                                      // Nbr tx   ARP pkts discarded.
} NET_CTR_ARP_ERRS;

/*--------------------------------------------------------------------------------------------------------
 *                                           NDP ERROR COUNTERS
 *------------------------------------------------------------------------------------------------------*/

typedef  struct  net_ctr_ndp_errs {
  NET_CTR NoneAvailCtr;                                     // Nbr unavail NDP cache      accesses.
  NET_CTR RouterNoneAvailCtr;                               // Nbr unavail NDP Router      pool accesses.
  NET_CTR PrefixNoneAvailCtr;                               // Nbr unavail NDP Prefix      pool accesses.
  NET_CTR DestCacheNoneAvailCtr;                            // Nbr unavail NDP Dest. cache pool accesses.
  NET_CTR InvTypeCtr;
  NET_CTR RxInvalidAddrSrc;
  NET_CTR RxInvalidAddrDest;
  NET_CTR RxInvalidAddrTarget;
  NET_CTR RxInvalidAddrLen;
  NET_CTR RxInvalidHopLimit;
  NET_CTR RxInvalidOptLen;
  NET_CTR RxInvalidOptType;
  NET_CTR RxInvalidPrefix;
  NET_CTR RxAddrHWThisHost;
  NET_CTR RxAddrDuplicate;
  NET_CTR RxAddrTentative;
  NET_CTR RxPrefixAddrCfgFail;
  NET_CTR RxNeighborAdvAddrDuplicateCtr;
  NET_CTR RxInvalidRouterAdvCtr;
} NET_CTR_NDP_ERRS;

/*--------------------------------------------------------------------------------------------------------
 *                                           CACHE ERROR COUNTERS
 *------------------------------------------------------------------------------------------------------*/

typedef  struct  net_ctr_cache_errs {
  NET_CTR ReTxPktFailCtr;
  NET_CTR TxPktDisCtr;
  NET_CTR NotUsedCtr;                                       // Nbr unused  cache      accesses.
}NET_CTR_CACHE_ERRS;

/*--------------------------------------------------------------------------------------------------------
 *                                           IPv4 ERROR COUNTERS
 *------------------------------------------------------------------------------------------------------*/

typedef  struct  net_ctr_ipv4_errs {
  NET_CTR CfgInvAddrHostCtr;                                // Nbr invalid IPv4 host         addr cfg attempts.
  NET_CTR CfgInvGatewayCtr;                                 // Nbr invalid IPv4 dflt gateway addr cfg attempts.
  NET_CTR CfgInvAddrInUseCtr;                               // Nbr in use  IPv4 host         addr cfg attempts.

  NET_CTR CfgAddrStateCtr;                                  // Nbr invalid IPv4 addr cfg state     accesses.
  NET_CTR CfgAddrNotFoundCtr;                               // Nbr invalid IPv4 addr NOT found     accesses.
  NET_CTR CfgAddrTblSizeCtr;                                // Nbr invalid IPv4 addr cfg tbl size  accesses.
  NET_CTR CfgAddrTblEmptyCtr;                               // Nbr invalid IPv4 addr cfg tbl empty accesses.
  NET_CTR CfgAddrTblFullCtr;                                // Nbr invalid IPv4 addr cfg tbl full  accesses.

  NET_CTR RxPktDisCtr;                                      // Nbr rx'd IPv4 pkts discarded.
  NET_CTR RxInvVerCtr;                                      // Nbr rx'd IPv4 datagrams with inv IP ver.
  NET_CTR RxInvLenCtr;                                      // Nbr rx'd IPv4 datagrams with inv hdr len.
  NET_CTR RxInvTotLenCtr;                                   // Nbr rx'd IPv4 datagrams with inv/inconsistent tot len.
  NET_CTR RxInvFlagsCtr;                                    // Nbr rx'd IPv4 datagrams with inv flags.
  NET_CTR RxInvFragCtr;                                     // Nbr rx'd IPv4 datagrams with inv fragmentation.
  NET_CTR RxInvProtocolCtr;                                 // Nbr rx'd IPv4 datagrams with inv/unsupported protocol.
  NET_CTR RxInvChkSumCtr;                                   // Nbr rx'd IPv4 datagrams with inv chk sum.
  NET_CTR RxInvAddrSrcCtr;                                  // Nbr rx'd IPv4 datagrams with inv src addr.
  NET_CTR RxInvOptsCtr;                                     // Nbr rx'd IPv4 datagrams with unknown/invalid opts.
  NET_CTR RxOptsBufNoneAvailCtr;                            // Nbr rx'd IPv4 datagrams with no options buf avail.
  NET_CTR RxOptsBufWrCtr;                                   // Nbr rx'd IPv4 datagrams with wr options buf err.
  NET_CTR RxDestCtr;                                        // Nbr rx'd IPv4 datagrams NOT for this IP dest.
  NET_CTR RxDestBcastCtr;                                   // Nbr rx'd IPv4 datagrams illegally broadcast to this dest.
  NET_CTR RxFragSizeCtr;                                    // Nbr rx'd IPv4 frags with invalid size.
  NET_CTR RxFragDisCtr;                                     // Nbr rx'd IPv4 frags            discarded.
  NET_CTR RxFragDgramDisCtr;                                // Nbr rx'd IPv4 frag'd datagrams discarded.
  NET_CTR RxFragDgramTimeoutCtr;                            // Nbr rx'd IPv4 frag'd datagrams timed out.

  NET_CTR TxPktDisCtr;                                      // Nbr tx   IPv4 pkts discarded.
  NET_CTR TxInvProtocolCtr;                                 // Nbr tx   IPv4 pkts with invalid/unsupported protocol.
  NET_CTR TxInvOptTypeCtr;                                  // Nbr tx   IPv4 pkts with invalid opt type.
  NET_CTR TxInvDestCtr;                                     // Nbr tx   IPv4 datagrams with invalid dest addr.
  NET_CTR TxFragNotSupported;

  NET_CTR TxInvOptLenCtr;                                   // Nbr tx   IPv4 datagrams with invalid opt len.
  NET_CTR TxInvOptCfgCtr;                                   // Nbr tx   IPv4 datagrams with invalid opt cfg.
  NET_CTR TxInvAddrSrcCtr;                                  // Nbr tx   IPv4 datagrams with invalid src  addr.
  NET_CTR TxInvAddrDestCtr;                                 // Nbr tx   IPv4 datagrams with invalid dest addr.

  NET_CTR HdrOptNotSupportedCtr;
} NET_CTR_IPv4_ERRS;

/*--------------------------------------------------------------------------------------------------------
 *                                           IPv6 ERROR COUNTERS
 *------------------------------------------------------------------------------------------------------*/

typedef  struct  net_ctr_ipv6_errs {
  NET_CTR CfgInvAddrHostCtr;                                // Nbr invalid IPv6 host         addr cfg attempts.
  NET_CTR CfgInvAddrInUseCtr;                               // Nbr in use  IPv6 host         addr cfg attempts.

  NET_CTR CfgAddrStateCtr;                                  // Nbr invalid IPv6 addr cfg state     accesses.
  NET_CTR CfgAddrNotFoundCtr;                               // Nbr invalid IPv6 addr NOT found     accesses.
  NET_CTR CfgAddrTblSizeCtr;                                // Nbr invalid IPv6 addr cfg tbl size  accesses.
  NET_CTR CfgAddrTblEmptyCtr;                               // Nbr invalid IPv6 addr cfg tbl empty accesses.
  NET_CTR CfgAddrTblFullCtr;                                // Nbr invalid IPv6 addr cfg tbl full  accesses.

  NET_CTR RxPktDisCtr;                                      // Nbr rx'd IPv6 pkts discarded.
  NET_CTR RxInvVerCtr;                                      // Nbr rx'd IPv6 datagrams with inv IP ver.
  NET_CTR RxInvTrafficClassCtr;                             // Nbr rx'd IPv6 datagrams with inv traffic class.
  NET_CTR RxInvFlowLabelCtr;                                // Nbr rx'd IPv6 datagrams with inv flow label.
  NET_CTR RxInvLenCtr;                                      // Nbr rx'd IPv6 datagrams with inv hdr len.
  NET_CTR RxInvTotLenCtr;                                   // Nbr rx'd IPv6 datagrams with inv/inconsistent tot len.
  NET_CTR RxInvProtocolCtr;                                 // Nbr rx'd IPv6 datagrams with inv/unsupported protocol.
  NET_CTR RxInvAddrSrcCtr;                                  // Nbr rx'd IPv6 datagrams with inv src addr.
  NET_CTR RxInvOptsCtr;                                     // Nbr rx'd IPv6 datagrams with unknown/invalid opts.
  NET_CTR RxInvOptsSeqCtr;
  NET_CTR RxOptsUnsupportedCtr;
  NET_CTR RxInvDestCtr;                                     // Nbr rx'd IPv6 datagrams NOT for this IP dest.
  NET_CTR RxFragSizeCtr;                                    // Nbr rx'd IPv6 frags with invalid size.
  NET_CTR RxFragTotLenCtr;
  NET_CTR RxFragDisCtr;                                     // Nbr rx'd IPv6 frags            discarded.
  NET_CTR RxFragDgramDisCtr;                                // Nbr rx'd IPv6 frag'd datagrams discarded.
  NET_CTR RxFragDgramTimeoutCtr;                            // Nbr rx'd IPv6 frag'd datagrams timed out.

  NET_CTR TxPktDisCtr;                                      // Nbr tx   IPv6 pkts discarded.
  NET_CTR TxInvProtocolCtr;                                 // Nbr tx   IPv6 pkts with invalid/unsupported protocol.
  NET_CTR TxFragNotSupported;
  NET_CTR TxInvAddrSrcCtr;                                  // Nbr tx   IPv6 datagrams with invalid src  addr.
  NET_CTR TxInvTTL_Ctr;

  NET_CTR ExtHdrNotSupported;

  NET_CTR AddrStaticCfgFaultCtr;                            // Nbr of Static Address Cfg that failed.
  NET_CTR AddrAutoCfgFaultCtr;                              // Nbr of Auto-Cfg Address that failed.
} NET_CTR_IPv6_ERRS;

/*--------------------------------------------------------------------------------------------------------
 *                                           ICMPv4 ERROR COUNTERS
 *------------------------------------------------------------------------------------------------------*/

typedef  struct  net_ctr_icmpv4_errs {
  NET_CTR RxPktDiscardedCtr;                                // Nbr rx'd ICMPv4 pkts discarded.
  NET_CTR RxInvTypeCtr;                                     // Nbr rx'd ICMPv4 msgs with unknown/invalid      msg type.
  NET_CTR RxInvCodeCtr;                                     // Nbr rx'd ICMPv4 msgs with unknown/invalid      msg code.
  NET_CTR RxInvMsgLenCtr;                                   // Nbr rx'd ICMPv4 msgs with invalid/inconsistent msg len.
  NET_CTR RxInvPtrCtr;                                      // Nbr rx'd ICMPv4 msgs with invalid              msg ptr.
  NET_CTR RxInvChkSumCtr;                                   // Nbr rx'd ICMPv4 msgs with invalid chk sum.
  NET_CTR RxBcastCtr;                                       // Nbr rx'd ICMPv4 msg  reqs rx'd via broadcast.
  NET_CTR RxMcastCtr;                                       // Nbr rx'd ICMPv4 msg  reqs rx'd via multicast.
  NET_CTR RxInvEchoReplyCtr;

  NET_CTR TxInvalidLenCtr;                                  // Nbr tx   ICMPv4 pkts discarded for invalid len.
  NET_CTR TxHdrTypeCtr;                                     // Nbr tx   ICMPv4 msgs with unknown/invalid msg type.
  NET_CTR TxHdrCodeCtr;                                     // Nbr tx   ICMPv4 msgs with unknown/invalid msg code.
  NET_CTR TxInvalidBroadcast;
  NET_CTR TxInvalidMulticast;
  NET_CTR TxInvalidFrag;
  NET_CTR TxPktDiscardedCtr;                                // Nbr tx   ICMPv4 pkts discarded.
} NET_CTR_ICMPv4_ERRS;

/*--------------------------------------------------------------------------------------------------------
 *                                           ICMPv6 ERROR COUNTERS
 *------------------------------------------------------------------------------------------------------*/

typedef  struct  net_ctr_icmp_v6 {
  NET_CTR RxInvTypeCtr;
  NET_CTR RxHdrTypeCtr;                                     // Nbr rx'd ICMPv6 msgs with unknown/invalid      msg type.
  NET_CTR RxHdrCodeCtr;                                     // Nbr rx'd ICMPv6 msgs with unknown/invalid      msg code.
  NET_CTR RxHdrMsgLenCtr;                                   // Nbr rx'd ICMPv6 msgs with invalid/inconsistent msg len.
  NET_CTR RxHdrPtrCtr;                                      // Nbr rx'd ICMPv6 msgs with invalid              msg ptr.
  NET_CTR RxHdrChkSumCtr;                                   // Nbr rx'd ICMPv6 msgs with invalid chk sum.
  NET_CTR RxInvEchoReplyCtr;

  NET_CTR RxMcastCtr;                                       // Nbr rx'd ICMPv6 msg  reqs rx'd via multicast.
  NET_CTR RxPktDiscardedCtr;                                // Nbr rx'd ICMPv6 pkts discarded.
  NET_CTR RxInvalidProtocolCtr;                             // Nbr rx'd ICMPv6 pkts with invalid/unsupported protocol.
  NET_CTR RxInvalidBufIxCtr;                                // Nbr rx   ICMPv6 pkts with invalid buf ix.
  NET_CTR RxHdrDataLenCtr;                                  // Nbr rx'd ICMPv6 msgs with invalid msg data len.

  NET_CTR TxInvalidLenCtr;                                  // Nbr tx   ICMPv6 pkts discarded for invalid len.
  NET_CTR TxHdrTypeCtr;                                     // Nbr tx   ICMPv6 msgs with unknown/invalid msg type.
  NET_CTR TxHdrCodeCtr;                                     // Nbr tx   ICMPv6 msgs with unknown/invalid msg code.
  NET_CTR TxPktDiscardedCtr;                                // Nbr tx   ICMPv6 pkts discarded.
  NET_CTR TxInvalidBufIxCtr;                                // Nbr tx   ICMPv6 pkts with invalid buf ix.
  NET_CTR TxHdrPtrCtr;                                      // Nbr tx   ICMPv6 msgs with invalid msg ptr.
  NET_CTR TxInvalidBroadcast;
  NET_CTR TxInvalidMulticast;
  NET_CTR TxInvalidFrag;
} NET_CTR_ICMPv6_ERRS;

/*--------------------------------------------------------------------------------------------------------
 *                                           IGMP ERROR COUNTERS
 *------------------------------------------------------------------------------------------------------*/

typedef  struct  net_ctr_igmp_errs {
  NET_CTR NoneAvailCtr;                                     // Nbr unavail IGMP host group accesses.

  NET_CTR RxHdrTypeCtr;                                     // Nbr rx'd IGMP msgs with unknown/invalid msg type.
  NET_CTR RxHdrVerCtr;                                      // Nbr rx'd IGMP msgs with invalid IGMP ver.
  NET_CTR RxHdrMsgLenCtr;                                   // Nbr rx'd IGMP msgs with invalid msg len.
  NET_CTR RxHdrChkSumCtr;                                   // Nbr rx'd IGMP msgs with invalid chk sum.
  NET_CTR RxPktInvalidAddrDestCtr;                          // Nbr rx'd IGMP msgs with invalid dest addr.
  NET_CTR RxPktDiscardedCtr;                                // Nbr rx'd IGMP pkts discarded.

  NET_CTR TxInvalidBufIxCtr;                                // Nbr rx   IGMP pkts with invalid buf ix.
  NET_CTR TxPktDiscardedCtr;                                // Nbr tx   IGMP pkts discarded.
} NET_CTR_IGMP_ERRS;

/*--------------------------------------------------------------------------------------------------------
 *                                           MLDP ERROR COUNTERS
 *------------------------------------------------------------------------------------------------------*/

typedef  struct  net_ctr_mldp_errs {
  NET_CTR NoneAvailCtr;                                     // Nbr unavail MLDP host group accesses.
  NET_CTR RxInvalidType;
  NET_CTR RxInvalidHopLimit;
  NET_CTR RxInvalidLen;
  NET_CTR RxInvalidAddrSrc;
  NET_CTR RxInvalidAddrDest;
  NET_CTR RxInvalidAddrGrp;
  NET_CTR RxInvalidHdr;
  NET_CTR RxGrpNoFound;
  NET_CTR TxPktDisCtr;                                      // Nbr tx      MLDP pkts discarded.
} NET_CTR_MLDP_ERRS;

/*--------------------------------------------------------------------------------------------------------
 *                                           UDP ERROR COUNTERS
 *------------------------------------------------------------------------------------------------------*/

typedef  struct  net_ctr_udp_errs {
  NET_CTR InvalidFlagsCtr;                                  // Nbr reqs           for  invalid UDP flags.
  NET_CTR RxHdrDatagramLenCtr;                              // Nbr rx'd UDP datagrams with invalid len.
  NET_CTR RxHdrPortSrcCtr;                                  // Nbr rx'd UDP datagrams with invalid src  port.
  NET_CTR RxHdrPortDestCtr;                                 // Nbr rx'd UDP datagrams with invalid dest port.
  NET_CTR RxHdrChkSumCtr;                                   // Nbr rx'd UDP datagrams with invalid chk sum.
  NET_CTR RxDestCtr;                                        // Nbr rx'd UDP datagrams for  unavail dest.
  NET_CTR RxPktDiscardedCtr;                                // Nbr rx'd UDP pkts discarded.
  NET_CTR RxHdrDataLenCtr;                                  // Nbr rx'd UDP datagrams with invalid data len.
  NET_CTR TxPktDiscardedCtr;                                // Nbr tx   UDP pkts discarded.
} NET_CTR_UDP_ERRS;

/*--------------------------------------------------------------------------------------------------------
 *                                           TCP ERROR COUNTERS
 *------------------------------------------------------------------------------------------------------*/

typedef  struct net_ctr_tcp_errs {
  NET_CTR NoneAvailCtr;                                         // Nbr unavail TCP conn      accesses.
  NET_CTR NotUsedCtr;                                           // Nbr unused  TCP conn      accesses.

  NET_CTR RxHdrLenCtr;                                          // Nbr rx'd TCP segs with invalid hdr len.
  NET_CTR RxHdrSegLenCtr;                                       // Nbr rx'd TCP segs with invalid seg len.
  NET_CTR RxHdrPortSrcCtr;                                      // Nbr rx'd TCP segs with invalid src  port.
  NET_CTR RxHdrPortDestCtr;                                     // Nbr rx'd TCP segs with invalid dest port.
  NET_CTR RxHdrAddrSrcCtr;                                      // Nbr rx'd TCP segs with invalid src  address.
  NET_CTR RxHdrFlagsCtr;                                        // Nbr rx'd TCP segs with invalid flags.
  NET_CTR RxHdrChkSumCtr;                                       // Nbr rx'd TCP segs with invalid chk sum.
  NET_CTR RxHdrOptsCtr;                                         // Nbr rx'd TCP segs with unknown/invalid opts.
  NET_CTR RxDestCtr;                                            // Nbr rx'd TCP segs for  unavail dest.
  NET_CTR RxPktDiscardedCtr;                                    // Nbr rx'd TCP pkts discarded.

  NET_CTR RxHdrFlagSynCtr;
  NET_CTR RxHdrFlagFinCtr;
  NET_CTR RxHdrFlagAckCtr;
  NET_CTR RxHdrFlagRstCtr;

  NET_CTR RxSeqInvalidCtr;
  NET_CTR RxAckInvalidCtr;
  NET_CTR RxDataInvalidCtr;
  NET_CTR RxDataDupCtr;

  NET_CTR RxAckNoneCtr;

  NET_CTR RxListenQ_FullCtr;

  NET_CTR RxPktQ_FullCtr;                                       // Nbr of pkt received with a zero window

  NET_CTR TxOptTypeCtr;                                         // Nbr tx   TCP pkts with invalid opt type.
  NET_CTR TxPktDiscardedCtr;                                    // Nbr tx   TCP pkts discarded.
  NET_CTR ReTxBufLock_Ctr;

  NET_CTR ConnInvalidCtr;                                       // Nbr invalid TCP conn ID   accesses.
  NET_CTR ConnInvalidOpCtr;                                     // Nbr invalid TCP conn ops.
  NET_CTR ConnInvalidStateCtr;                                  // Nbr invalid TCP conn states.
  NET_CTR ConnCloseCtr;                                         // Nbr fault   TCP conn closes.

  NET_CTR TxHdrOptLenCtr;                                       // Nbr tx   TCP segs with invalid opt len.
} NET_CTR_TCP_ERRS;

/*--------------------------------------------------------------------------------------------------------
 *                                           SOCKET ERROR COUNTERS
 *------------------------------------------------------------------------------------------------------*/

typedef  struct  net_ctr_sock_errs {
  NET_CTR NullSizeCtr;
  NET_CTR NoneAvailCtr;                                     // Nbr unavail sock      accesses.
  NET_CTR NotUsedCtr;                                       // Nbr unused  sock      accesses.
  NET_CTR CloseCtr;                                         // Nbr fault   sock closes.

  NET_CTR InvalidFamilyCtr;                                 // Nbr socks with invalid sock family.
  NET_CTR InvalidProtocolCtr;                               // Nbr socks with invalid sock protocol.
  NET_CTR InvalidSockTypeCtr;                               // Nbr socks with invalid sock type.
  NET_CTR InvalidSockCtr;                                   // Nbr            invalid sock ID accesses.
  NET_CTR InvalidFlagsCtr;                                  // Nbr socks with invalid flags.
  NET_CTR InvalidOpCtr;                                     // Nbr socks with invalid op.
  NET_CTR InvalidStateCtr;                                  // Nbr socks with invalid state.
  NET_CTR InvalidAddrCtr;                                   // Nbr socks with invalid addr.
  NET_CTR InvalidAddrLenCtr;                                // Nbr socks with invalid addr len.
  NET_CTR InvalidAddrInUseCtr;                              // Nbr socks with         addr already in use.
  NET_CTR InvalidPortNbrCtr;                                // Nbr socks with invalid port nbr.
  NET_CTR InvalidConnInUseCtr;                              // Nbr socks with         conn already in use.

    #ifdef  NET_SOCK_TYPE_STREAM_MODULE_EN
  NET_CTR ConnAcceptQ_NoneAvailCtr;                         // Nbr unavail sock accept Q conn   accesses.
    #endif
  NET_CTR RandomPortNbrNoneAvailCtr;                        // Nbr unavail sock random port nbr accesses.

  NET_CTR RxDestCtr;                                        // Nbr rx'd sock pkts for unavail dest.
  NET_CTR RxPktDiscardedCtr;                                // Nbr rx'd sock pkts discarded.
} NET_CTR_SOCK_ERRS;

/*--------------------------------------------------------------------------------------------------------
 *                                           ERROR COUNTERS
 *------------------------------------------------------------------------------------------------------*/
typedef  struct  net_ctr_errs {
  NET_CTR_TMR_ERRS  Tmr;
  NET_CTR_BUF_ERRS  Buf;
  NET_CTR_CONN_ERRS Conn;
  NET_CTR_IFs_ERRS  IFs;

    #ifdef  NET_ARP_MODULE_EN
  NET_CTR_ARP_ERRS  ARP;
    #endif

    #ifdef  NET_NDP_MODULE_EN
  NET_CTR_NDP_ERRS NDP;
    #endif

    #ifdef  NET_CACHE_MODULE_EN
  NET_CTR_CACHE_ERRS Cache;
    #endif

    #ifdef  NET_IPv4_MODULE_EN
  NET_CTR_IPv4_ERRS IPv4;
    #endif

    #ifdef  NET_IPv6_MODULE_EN
  NET_CTR_IPv6_ERRS IPv6;
    #endif

    #ifdef  NET_ICMPv4_MODULE_EN
  NET_CTR_ICMPv4_ERRS ICMPv4;
    #endif

    #ifdef  NET_ICMPv6_MODULE_EN
  NET_CTR_ICMPv6_ERRS ICMPv6;
    #endif

    #ifdef  NET_IGMP_MODULE_EN
  NET_CTR_IGMP_ERRS IGMP;
    #endif

    #ifdef  NET_MLDP_MODULE_EN
  NET_CTR_MLDP_ERRS MLDP;
    #endif

  NET_CTR_UDP_ERRS  UDP;

    #ifdef  NET_TCP_MODULE_EN
  NET_CTR_TCP_ERRS  TCP;
    #endif

  NET_CTR_SOCK_ERRS Sock;
} NET_CTR_ERRS;

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

/*--------------------------------------------------------------------------------------------------------
 *                                           STATISTIC COUNTERS
 *------------------------------------------------------------------------------------------------------*/

#if (NET_CTR_CFG_STAT_EN == DEF_ENABLED)
extern NET_CTR_STATS Net_StatCtrs;
#endif

/*--------------------------------------------------------------------------------------------------------
 *                                           ERROR COUNTERS
 *------------------------------------------------------------------------------------------------------*/

#if (NET_CTR_CFG_ERR_EN == DEF_ENABLED)
extern NET_CTR_ERRS Net_ErrCtrs;
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MACRO'S
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                       NETWORK COUNTER MACRO'S
 *
 * Description : Handle network counter(s).
 *
 * Argument(s) : Various network counter variable(s) & values.
 *
 * Return(s)   : none.
 *
 * Note(s)     : (1) Network counter variables MUST ALWAYS be accessed exclusively in critical sections.
 *
 *                   Therefore, local variable 'cpu_sr' MUST be declared via the CPU_SR_ALLOC() macro
 *                   in the following functions in case the CPU critical section method is configured
 *                   as 'CPU_CRITICAL_METHOD_STATUS_LOCAL' :
 *
 *                   (a) NetCtr_Inc()
 *                   (b) ALL functions which call NET_CTR_&&&_INC_LARGE() macro's
 *******************************************************************************************************/

#define  NET_CTR_INC(ctr)                                       NetCtr_Inc(&(ctr));

#define  NET_CTR_INC_LARGE(ctr_hi, ctr_lo)                do {  CORE_ENTER_ATOMIC();        \
                                                                NetCtr_IncLarge(&(ctr_hi),  \
                                                                                &(ctr_lo)); \
                                                                CORE_EXIT_ATOMIC();        } while (0)

#define  NET_CTR_ADD(ctr, val)                            do { (ctr) += (val); } while (0)

#if     (NET_CTR_CFG_STAT_EN == DEF_ENABLED)
    #define  NET_CTR_STAT_INC(stat_ctr)                             NET_CTR_INC(stat_ctr)
    #define  NET_CTR_STAT_INC_LARGE(stat_ctr_hi, stat_ctr_lo)       NET_CTR_INC_LARGE(stat_ctr_hi, stat_ctr_lo)

    #define  NET_CTR_STAT_ADD(stat_ctr, val)                        NET_CTR_ADD(stat_ctr, val)

#else
    #define  NET_CTR_STAT_INC(stat_ctr)
    #define  NET_CTR_STAT_INC_LARGE(stat_ctr_hi, stat_ctr_lo)

    #define  NET_CTR_STAT_ADD(stat_ctr, val)

#endif

#if     (NET_CTR_CFG_ERR_EN == DEF_ENABLED)

    #define  NET_CTR_ERR_INC(err_ctr)                               NET_CTR_INC(err_ctr)
    #define  NET_CTR_ERR_INC_LARGE(err_ctr_hi, err_ctr_lo)          NET_CTR_INC_LARGE(err_ctr_hi, err_ctr_lo)

#else

    #define  NET_CTR_ERR_INC(err_ctr)
    #define  NET_CTR_ERR_INC_LARGE(err_ctr_hi, err_ctr_lo)

#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                           FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

void NetCtr_Init(MEM_SEG  *p_mem_seg,
                 RTOS_ERR *p_err);

//                                                                 -------------- CTR API FNCTS ---------------
#ifdef NET_CTR_MODULE_EN
void NetCtr_Inc(NET_CTR *p_ctr);

void NetCtr_IncLarge(NET_CTR *p_ctr_hi,
                     NET_CTR *p_ctr_lo);
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // _NET_CTR_PRIV_H_
