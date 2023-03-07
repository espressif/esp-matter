/***************************************************************************//**
 * @file
 * @brief Network Defines
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

#ifndef  _NET_DEF_PRIV_H_
#define  _NET_DEF_PRIV_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                            INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <cpu/include/cpu.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                           NETWORK DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                NETWORK MODULE & LAYER GLOBAL DEFINES
 ********************************************************************************************************
 ********************************************************************************************************
 *
 * Note(s) : (1) These module & layer global #define's are required PRIOR to network configuration.
 *******************************************************************************************************/

/********************************************************************************************************
 *                                   NETWORK INTERFACE LAYER DEFINES
 *
 * Note(s) : (1) See specific 'net_if_&&&.h  NETWORK INTERFACE HEADER DEFINES'.
 *******************************************************************************************************/

#define  NET_IF_NBR_IF_RESERVED               1
#define  NET_IF_NBR_IF_TOT                 (NET_IF_CFG_MAX_NBR_IF + NET_IF_NBR_IF_RESERVED)

//                                                                 See Note #1.
#define  NET_IF_HDR_SIZE_LOOPBACK                         NET_IF_NONE_HW_ADD_LEN
#define  NET_IF_HDR_SIZE_ETHER                            14

#define  NET_IF_LOOPBACK_SIZE_MIN                          0

/********************************************************************************************************
 *                                       CACHE MGR LAYER DEFINES
 *
 * Note(s) : (1) See 'net_cache.h  ARP HARDWARE & PROTOCOL DEFINES  Note #1'.
 *******************************************************************************************************/

#define  NET_CACHE_HW_ADDR_LEN_ETHER                       6

/********************************************************************************************************
 *                                          ARP LAYER DEFINES
 *
 * Note(s) : (1) See 'net_arp.h  ARP HARDWARE & PROTOCOL DEFINES  Note #1'.
 *******************************************************************************************************/

#define  NET_ARP_HDR_SIZE_MIN                             12
#define  NET_ARP_HDR_SIZE_MAX                             68
#define  NET_ARP_HDR_SIZE_BASE                             8
//                                                                 See Note #1a.
#define  NET_ARP_HDR_SIZE                               (NET_ARP_HDR_SIZE_BASE           \
                                                         + (NET_IF_HW_ADDR_LEN_MAX  * 2) \
                                                         + (NET_IPv4_ADDR_SIZE      * 2))

/********************************************************************************************************
 *                                          IP LAYER DEFINES
 *
 * Note(s) : (1) See 'net_ip.h  IP MULTICAST SELECT DEFINES  Note #1'.
 *******************************************************************************************************/
//                                                                 ------------------ IP VERSION SEL ------------------
//                                                                 See Note #1.
#define  NET_IP_VER_SEL_IPv4                               0u
#define  NET_IP_VER_SEL_IPv6                               1u
#define  NET_IP_VER_SEL_IPv4_IPv6                          2u

//                                                                 ----------------- IP MULTICAST SEL -----------------
//                                                                 See Note #2.
#define  NET_IGMP_MCAST_SEL_NONE                         0u
#define  NET_IGMP_MCAST_SEL_TX                           1u
#define  NET_IGMP_MCAST_SEL_TX_RX                        2u

#define  NET_IP_HW_ADDR_LEN                                NET_IF_HW_ADDR_LEN_MAX

/********************************************************************************************************
 *                                         IPv4 LAYER DEFINES
 *
 * Note(s) : (1) See 'net_ipv4.h  IPv4 MULTICAST SELECT DEFINES  Note #1'.
 *******************************************************************************************************/

#define  NET_IPv4_HDR_LEN_MIN                              5
#define  NET_IPv4_HDR_LEN_MAX                             15
#define  NET_IPv4_HDR_LEN_WORD_SIZE                      CPU_WORD_SIZE_32

#define  NET_IPv4_HDR_SIZE_MIN                          (NET_IPv4_HDR_LEN_MIN * NET_IPv4_HDR_LEN_WORD_SIZE)

#define  NET_IPv4_HDR_SIZE_MAX                            60    // See 'net_ipv4.h  IPv4 HEADER DEFINES  Note #1'.

/********************************************************************************************************
 *                                         IPv6 LAYER DEFINES
 *
 * Note(s) : (1) See 'net_ipv6.h  IPv6 MULTICAST SELECT DEFINES  Note #1'.
 *******************************************************************************************************/

#define  NET_IPv6_HDR_SIZE_MAX                            40    // See 'net_ipv6.h  IPv6 HEADER DEFINES  Note #1'.

#define  NET_IPv6_CFG_MLDP_VER_V1                         1u
#define  NET_IPv6_CFG_MLDP_VER_V2                         2u

#define  NET_IPv6_HDR_SIZE                               40u

/********************************************************************************************************
 *                                       TRANSPORT LAYER DEFINES
 *
 * Note(s) : (1) The following transport layer values are pre-#define'd in 'net_type.h' (see 'net_type.h
 *               NETWORK TRANSPORT LAYER PORT NUMBER DATA TYPE  Note #1') :
 *
 *               (a) NET_PORT_NBR_MAX                     DEF_INT_16U_MAX_VAL
 *******************************************************************************************************/

//                                                                 ------------- NET TRANSPORT LAYER SEL --------------
#define  NET_TRANSPORT_LAYER_SEL_NONE                      0u
#define  NET_TRANSPORT_LAYER_SEL_UDP                       1u
#define  NET_TRANSPORT_LAYER_SEL_UDP_TCP                   2u

//                                                                 ------------------ NET PORT NBRS -------------------
#define  NET_PORT_NBR_RESERVED                             0u
#define  NET_PORT_NBR_NONE                               NET_PORT_NBR_RESERVED

#define  NET_PORT_NBR_MIN                                  1u
#if 0                                                           // See Note #1a.
#define  NET_PORT_NBR_MAX                                DEF_INT_16U_MAX_VAL
#endif

/********************************************************************************************************
 *                                          UDP LAYER DEFINES
 *
 * Note(s) : (1) See 'net_udp.h  UDP APPLICATION-PROTOCOL-INTERFACE SELECT DEFINES  Note #1'.
 *******************************************************************************************************/

#define  NET_UDP_HDR_SIZE_MIN                              8    // See 'net_udp.h  UDP HEADER DEFINES  Note #1a'.
#define  NET_UDP_HDR_SIZE_MAX                              8    // See 'net_udp.h  UDP HEADER DEFINES  Note #1b'.

#define  NET_UDP_HDR_SIZE                                  8

//                                                                 ----------------- UDP/APP API SEL ------------------
//                                                                 See Note #1.
#define  NET_UDP_APP_API_SEL_NONE                          0u
#define  NET_UDP_APP_API_SEL_SOCK                          1u
#define  NET_UDP_APP_API_SEL_APP                           2u
#define  NET_UDP_APP_API_SEL_SOCK_APP                      3u

/********************************************************************************************************
 *                                          IGMP LAYER DEFINES
 *******************************************************************************************************/

#define  NET_IGMP_HDR_SIZE                                 8
#define  NET_IGMP_MSG_SIZE_MIN             NET_IGMP_HDR_SIZE    // See Note #1.

/********************************************************************************************************
 *                                          TCP LAYER DEFINES
 *******************************************************************************************************/

#define  NET_TCP_HDR_LEN_MIN                               5u
#define  NET_TCP_HDR_LEN_MAX                              15
#define  NET_TCP_HDR_LEN_WORD_SIZE                       CPU_WORD_SIZE_32

#define  NET_TCP_HDR_SIZE_MIN                           (NET_TCP_HDR_LEN_MIN * NET_TCP_HDR_LEN_WORD_SIZE)
#define  NET_TCP_HDR_SIZE_MAX                             60    // See 'net_tcp.h  TCP HEADER DEFINES  Note #1'.

/********************************************************************************************************
 *                                NETWORK CONNECTION MANAGEMENT DEFINES
 *******************************************************************************************************/

//                                                                 ---------------- CONN FAMILY TYPES -----------------
#define  NET_CONN_FAMILY_NONE                              0
//                                                                 Net sock cfg :
#define  NET_CONN_FAMILY_IP_V4_SOCK                      NET_SOCK_FAMILY_IP_V4
#define  NET_CONN_ADDR_LEN_IP_V4_SOCK                    NET_SOCK_ADDR_LEN_IP_V4
#define  NET_CONN_PROTOCOL_MAX_IP_V4_SOCK                NET_SOCK_PROTO_MAX_IP_V4

#define  NET_CONN_FAMILY_IP_V6_SOCK                      NET_SOCK_FAMILY_IP_V6
#define  NET_CONN_ADDR_LEN_IP_V6_SOCK                    NET_SOCK_ADDR_LEN_IP_V6
#define  NET_CONN_PROTOCOL_MAX_IP_V6_SOCK                NET_SOCK_PROTO_MAX_IP_V6

#define  NET_CONN_ID_NONE                                 -1

/********************************************************************************************************
 ********************************************************************************************************
 *                                             MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // _NET_DEF_PRIV_H_
