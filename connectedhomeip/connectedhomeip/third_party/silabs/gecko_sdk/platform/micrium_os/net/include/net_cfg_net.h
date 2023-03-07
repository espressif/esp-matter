/***************************************************************************//**
 * @file
 * @brief Network Internal Configuration
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

#ifndef  _NET_CFG_NET_H_
#define  _NET_CFG_NET_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                            INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <rtos_description.h>
#include  <net_cfg.h>

#include  <cpu/include/cpu.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                               NETWORK STATISTIC POOL CONFIGURATION
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef  NET_STAT_POOL_BUF_EN
    #define  NET_STAT_POOL_BUF_EN       DEF_DISABLED
#endif

#ifndef  NET_STAT_POOL_ARP_EN
    #define  NET_STAT_POOL_ARP_EN       DEF_DISABLED
#endif

#ifndef  NET_STAT_POOL_NDP_EN
    #define  NET_STAT_POOL_NDP_EN       DEF_DISABLED
#endif

#ifndef  NET_STAT_POOL_IGMP_EN
    #define  NET_STAT_POOL_IGMP_EN      DEF_DISABLED
#endif

#ifndef  NET_STAT_POOL_MLDP_EN
    #define  NET_STAT_POOL_MLDP_EN      DEF_DISABLED
#endif

#ifndef  NET_STAT_POOL_CONN_EN
    #define  NET_STAT_POOL_CONN_EN      DEF_DISABLED
#endif

#ifndef  NET_STAT_POOL_SOCK_EN
    #define  NET_STAT_POOL_SOCK_EN      DEF_DISABLED
#endif

#ifndef  NET_STAT_POOL_TCP_CONN_EN
    #define  NET_STAT_POOL_TCP_CONN_EN  DEF_DISABLED
#endif

#ifndef  NET_STAT_POOL_TMR_EN
    #define  NET_STAT_POOL_TMR_EN       DEF_DISABLED
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                NETWORK INTERFACE LAYER CONFIGURATION
 *
 * Note(s) : (1) The following pre-processor directives correctly configure network interface parameters.
 *               DO NOT MODIFY.
 *
 *           (2) (a) (1) Loopback interface  required only if internal loopback communication is enabled.
 *
 *                   (2) Specific interfaces required only if devices are configured for the interface(s).
 *
 *               (b) Some network interface share common code to receive and transmit packets, implemented in
 *                   an 802x Protocol Layer (see 'net_if_802x.h' Note #1).
 *
 *               (c) Some network interfaces require network-address-to-hardware-address bindings, implemented
 *                   in an Address Resolution Protocol Layer (see 'net_arp.h  Note #1').
 *
 *                   Ideally, the ARP Layer would configure the network protocol suite for the inclusion of
 *                   the ARP Layer via the NET_ARP_MODULE_EN #define (see 'net_arp.h  MODULE  Note #2'
 *                   &  'ARP LAYER CONFIGURATION  Note #2b').
 *
 *                   However, since the ARP Layer is required only for SOME network interfaces, the presence
 *                   of the ARP Layer MUST be configured ...
 *
 *                   (a) By each network interface that requires the     ARP Layer
 *                         AND
 *                   (b) PRIOR to all other network modules that require ARP Layer configuration
 *
 *           (3) Ideally, the Network Interface layer would define ALL network interface numbers.  However,
 *               certain network interface numbers MUST be defined PRIOR to all other network modules that
 *               require network interface numbers.
 *
 *               See also 'net_if.h  NETWORK INTERFACE NUMBER DATA TYPE  Note #2b'.
 ********************************************************************************************************
 *******************************************************************************************************/

//                                                                 ---------------- CFG NET IF PARAMS -----------------
#if (NET_IF_CFG_LOOPBACK_EN == DEF_ENABLED)
    #define  NET_IF_LOOPBACK_MODULE_EN                          // See Note #2a1.
#endif

//                                                                 See Note #2a2.
#ifdef RTOS_MODULE_NET_IF_ETHER_AVAIL
    #define  NET_IF_ETHER_MODULE_EN
#endif

#ifdef RTOS_MODULE_NET_IF_WIFI_AVAIL
    #define  NET_IF_WIFI_MODULE_EN
#endif

#if  (defined(NET_IF_ETHER_MODULE_EN) \
  || defined(NET_IF_WIFI_MODULE_EN)   \
  || defined(NET_IF_LOOPBACK_MODULE_EN))

    #define  NET_IF_802x_MODULE_EN                              // See Note #2b.
    #define  NET_IF_MODULE_EN
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                       IP LAYER CONFIGURATION
 *
 * Note(s) : (1) The following pre-processor directives correctly configure IP parameters.  DO NOT MODIFY.
 ********************************************************************************************************
 *******************************************************************************************************/

#if (NET_IPv4_CFG_EN == DEF_ENABLED)

    #define  NET_IPv4_MODULE_EN
    #define  NET_ICMPv4_MODULE_EN

    #if (defined(NET_IF_ETHER_MODULE_EN)) \
  || (defined(NET_IF_WIFI_MODULE_EN))
        #define  NET_ARP_MODULE_EN
    #endif

    #if (NET_MCAST_CFG_IPv4_RX_EN == DEF_ENABLED)
        #define  NET_IGMP_MODULE_EN

    #elif (NET_MCAST_CFG_IPv4_TX_EN == DEF_ENABLED)
        #define  NET_IGMP_MCAST_TX_MODULE_EN
    #endif

#ifndef  NET_IPV4_CFG_CHK_SUM_OFFLOAD_RX_EN
    #define  NET_IPV4_CFG_CHK_SUM_OFFLOAD_RX_EN                 DEF_DISABLED
#endif

#ifndef  NET_IPV4_CFG_CHK_SUM_OFFLOAD_TX_EN
    #define  NET_IPV4_CFG_CHK_SUM_OFFLOAD_TX_EN                 DEF_DISABLED
#endif

#if (NET_IPV4_CFG_CHK_SUM_OFFLOAD_RX_EN == DEF_ENABLED)
    #define  NET_IPV4_CHK_SUM_OFFLOAD_RX
#endif

#if (NET_IPV4_CFG_CHK_SUM_OFFLOAD_TX_EN == DEF_ENABLED)
    #define  NET_IPV4_CHK_SUM_OFFLOAD_TX
#endif

#if ((NET_IPv4_CFG_LINK_LOCAL_EN == DEF_ENABLED) \
  && (defined(NET_ARP_MODULE_EN)))
    #define NET_IPv4_LINK_LOCAL_MODULE_EN
#endif

#endif

#if (NET_IPv6_CFG_EN == DEF_ENABLED)

    #define  NET_IPv6_MODULE_EN
    #define  NET_ICMPv6_MODULE_EN
    #define  NET_MLDP_MODULE_EN
    #define  NET_NDP_MODULE_EN

    #if (NET_IPv6_CFG_DAD_EN == DEF_ENABLED)
        #define  NET_DAD_MODULE_EN
    #endif

    #if (NET_IPv6_CFG_ADDR_AUTO_CFG_EN == DEF_ENABLED)
        #define  NET_IPv6_ADDR_AUTO_CFG_MODULE_EN
    #endif

#endif

#if (defined(NET_IPv4_MODULE_EN) \
  || defined(NET_IPv6_MODULE_EN))
    #define  NET_IP_MODULE_EN

#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                       MULTICAST CONFIGURATION
 ********************************************************************************************************
 *******************************************************************************************************/

#if (defined(NET_IGMP_MODULE_EN) \
  || defined(NET_MLDP_MODULE_EN))

    #define  NET_MCAST_MODULE_EN
    #define  NET_MCAST_RX_MODULE_EN
    #define  NET_MCAST_TX_MODULE_EN

#elif (defined(NET_IGMP_MCAST_TX_MODULE_EN))
    #define  NET_MCAST_MODULE_EN
    #define  NET_MCAST_TX_MODULE_EN
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                        NETWORK ADDRESS CACHE MANAGEMENT LAYER CONFIGURATION
 *
 * Note(s) : (1) Network Address Cache Management layer required by some network layers (see 'net_cache.h  Note #1').
 ********************************************************************************************************
 *******************************************************************************************************/

#if ((defined(NET_ARP_MODULE_EN)) \
  || (defined(NET_NDP_MODULE_EN)))

    #define  NET_CACHE_MODULE_EN                               // See Note #1.

#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                       TCP LAYER CONFIGURATION
 *
 * Note(s) : (1) The following pre-processor directives correctly configure TCP parameters.  DO NOT MODIFY.
 *
 *           (2) (a) TCP Layer required only for some application interfaces (see 'net_tcp.h  MODULE
 *                   Note #1').
 *
 *               (b) Ideally, the TCP Layer would configure the network protocol suite for the inclusion of
 *                   the TCP Layer via the NET_TCP_MODULE_EN #define (see 'net_tcp.h  MODULE  Note #2').
 *                   However, the presence of the TCP Layer MUST be configured PRIOR to all other network
 *                   modules that require TCP Layer configuration.
 ********************************************************************************************************
 *******************************************************************************************************/

#if (NET_TCP_CFG_EN == DEF_ENABLED)
    #define  NET_TCP_MODULE_EN                                 // See Note #2.

    #ifndef  NET_TCP_CFG_CHK_SUM_OFFLOAD_RX_EN
        #define  NET_TCP_CFG_CHK_SUM_OFFLOAD_RX_EN              DEF_DISABLED
    #endif

    #ifndef  NET_TCP_CFG_CHK_SUM_OFFLOAD_TX_EN
        #define  NET_TCP_CFG_CHK_SUM_OFFLOAD_TX_EN              DEF_DISABLED
    #endif

    #if (NET_TCP_CFG_CHK_SUM_OFFLOAD_RX_EN == DEF_ENABLED)
        #define  NET_TCP_CHK_SUM_OFFLOAD_RX
    #endif

    #if (NET_TCP_CFG_CHK_SUM_OFFLOAD_TX_EN == DEF_ENABLED)
        #define  NET_TCP_CHK_SUM_OFFLOAD_TX
    #endif

#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                       UDP LAYER CONFIGURATION
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef  NET_UDP_CFG_CHK_SUM_OFFLOAD_RX_EN
    #define  NET_UDP_CFG_CHK_SUM_OFFLOAD_RX_EN                 DEF_DISABLED
#endif

#ifndef  NET_UDP_CFG_CHK_SUM_OFFLOAD_TX_EN
    #define  NET_UDP_CFG_CHK_SUM_OFFLOAD_TX_EN                 DEF_DISABLED
#endif

#if (NET_UDP_CFG_CHK_SUM_OFFLOAD_RX_EN == DEF_ENABLED)
    #define  NET_UDP_CHK_SUM_OFFLOAD_RX
#endif

#if (NET_UDP_CFG_CHK_SUM_OFFLOAD_TX_EN == DEF_ENABLED)
    #define  NET_UDP_CHK_SUM_OFFLOAD_TX
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                       ICMP CHECKSUM CONFIGURATION
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef  NET_ICMP_CFG_CHK_SUM_OFFLOAD_RX_EN
    #define  NET_ICMP_CFG_CHK_SUM_OFFLOAD_RX_EN                 DEF_DISABLED
#endif

#ifndef  NET_ICMP_CFG_CHK_SUM_OFFLOAD_TX_EN
    #define  NET_ICMP_CFG_CHK_SUM_OFFLOAD_TX_EN                 DEF_DISABLED
#endif

#if (NET_ICMP_CFG_CHK_SUM_OFFLOAD_RX_EN == DEF_ENABLED)
    #define  NET_ICMP_CHK_SUM_OFFLOAD_RX
#endif

#if (NET_ICMP_CFG_CHK_SUM_OFFLOAD_TX_EN == DEF_ENABLED)
    #define  NET_ICMP_CHK_SUM_OFFLOAD_TX
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                             NETWORK SOCKET LAYER & SECURITY CONFIGURATION
 *
 * Note(s) : (1) The following pre-processor directives correctly configure network socket parameters.
 *               DO NOT MODIFY.
 *
 *           (2) (a) Network Security Layer required only for some configurations (see 'net_secure_mgr.h
 *                   Note #1').
 *
 *               (b) Ideally, the Network Security Layer would configure the network protocol suite for
 *                   the inclusion of the Security Layer via the NET_SECURE_MODULE_EN #define (see
 *                   'net_secure_mgr.h  MODULE  Note #2').  However, the presence of the Security Layer
 *                   MUST be configured PRIOR to all other network modules that require  Security Layer
 *                   configuration.
 ********************************************************************************************************
 *******************************************************************************************************/

//                                                                 ------------ CFG SOCK MODULE INCLUSION -------------
#ifdef NET_TCP_MODULE_EN
    #define  NET_SOCK_TYPE_STREAM_MODULE_EN

//                                                                 ----------- CFG SECURE MODULE INCLUSION ------------
    #if (defined(RTOS_MODULE_NET_SSL_TLS_AVAIL) \
  && (defined(RTOS_MODULE_NET_SSL_TLS_MBEDTLS_AVAIL)))
        #define  NET_SECURE_MODULE_EN
    #endif
#endif

#ifndef  NET_SOCK_DFLT_BSD_EN
    #define NET_SOCK_BSD_EN
#else
    #if (NET_SOCK_DFLT_BSD_EN == DEF_ENABLED)
        #define NET_SOCK_BSD_EN
    #endif
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                               NETWORK COUNTER MANAGEMENT CONFIGURATION
 ********************************************************************************************************
 *******************************************************************************************************/

#if    ((NET_CTR_CFG_STAT_EN == DEF_ENABLED) \
  || (NET_CTR_CFG_ERR_EN == DEF_ENABLED))

    #define  NET_CTR_MODULE_EN
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                               NETWORK BUFFER MANAGEMENT CONFIGURATION
 *
 * Note(s) : (1) The following pre-processor directives correctly configure network buffer parameters.
 *               DO NOT MODIFY.
 *
 *               (a) NET_BUF_DATA_PROTOCOL_HDR_SIZE_MAX's ideal #define'tion :
 *
 *                       (A) IF Hdr  +  max(Protocol Headers)
 *
 *               (b) NET_BUF_DATA_PROTOCOL_HDR_SIZE_MAX  #define'd with hard-coded knowledge that IF, IP,
 *                   & TCP headers have the largest combined maximum size of all the protocol headers :
 *
 *                     ARP Hdr   68       IP Hdr   60      IP Hdr   60      IP Hdr   60     IP Hdr   60
 *                                      ICMP Hdr    0    IGMP Hdr    0     UDP Hdr    8    TCP Hdr   60
 *                     ------------     -------------    -------------     ------------    ------------
 *                     Total     68     Total      60    Total      60     Total     68    Total    120
 *
 *               See also 'net_buf.h  NETWORK BUFFER INDEX & SIZE DEFINES  Note #1'.
 *
 *           (3) Network interface minimum/maximum header sizes MUST be #define'd based on network interface
 *               type(s) configured in 'net_cfg.h'.  Assumes header sizes are fixed based on configured network
 *               interface type(s) [see any 'net_if_&&&.h  NETWORK INTERFACE HEADER DEFINES  Note #1'].
 ********************************************************************************************************
 *******************************************************************************************************/

#define  NET_BUF_MODULE_EN
#define  NET_TMR_MODULE_EN

/********************************************************************************************************
 ********************************************************************************************************
 *                                  NETWORK CONNECTION CONFIGURATION
 *
 * Note(s) : (1) The following pre-processor directives correctly configure network communication
 *               parameters.  DO NOT MODIFY.
 *
 *           (2) To balance network receive versus transmit packet loads for certain network connection
 *               types (e.g. stream-type connections), network receive & transmit packets SHOULD be
 *               handled in an APPROXIMATELY balanced ratio.
 *
 *               See also 'net_if.c  NetIF_RxPktInc()         Note #1',
 *                        'net_if.c  NetIF_RxPktDec()         Note #1',
 *                        'net_if.c  NetIF_TxSuspend()        Note #1',
 *                      & 'net_if.c  NetIF_TxSuspendSignal()  Note #1'.
 ********************************************************************************************************
 *******************************************************************************************************/

#ifdef NET_TCP_MODULE_EN
    #define  NET_LOAD_BAL_MODULE_EN
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                 NETWORK OTHER MODULES CONFIGURATION
 ********************************************************************************************************
 *******************************************************************************************************/

#ifdef NET_IPv4_MODULE_EN

#if ((NET_DHCP_CLIENT_CFG_MODULE_EN == DEF_ENABLED) \
  && (defined(NET_ARP_MODULE_EN)))
    #define  NET_DHCP_CLIENT_MODULE_EN
#endif

#endif

#if (NET_DNS_CLIENT_CFG_MODULE_EN == DEF_ENABLED)
    #define  NET_DNS_CLIENT_MODULE_EN
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                    NETWORK CONFIGURATION ERRORS
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef  NET_IP_MODULE_EN
#error  "NET_IPv4_CFG_EN and/or NET_IPv6_CFG_EN [MUST be ENABLED]"
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                             MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // _NET_CFG_NET_H_
