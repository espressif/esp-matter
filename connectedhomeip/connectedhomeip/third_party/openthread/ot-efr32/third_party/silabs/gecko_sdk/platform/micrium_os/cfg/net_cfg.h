/***************************************************************************//**
 * @file
 * @brief Network Configuration - Configuration Template File
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc.  Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement.  This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

// <<< Use Configuration Wizard in Context Menu >>>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef  _NET_CFG_H_
#define  _NET_CFG_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                  NETWORK OPTIMIZATION CONFIGURATION
 *
 * Note(s) : (1) TCP-IP code may call optimized assembly functions. Optimized assembly files/functions
 *               must be included in the project to be enabled.
 ********************************************************************************************************
 *******************************************************************************************************/

#define  NET_CFG_OPTIMIZE_ASM_EN                            0

/********************************************************************************************************
 ********************************************************************************************************
 *                                     NETWORK DEBUG CONFIGURATION
 *
 * Note(s) : (1) Configure NET_DBG_CFG_MEM_CLR_EN to enable/disable the network protocol suite from
 *               clearing internal data structure memory buffers; a convenient feature while debugging.
 *
 *           (2) Configure NET_CTR_CFG_STAT_EN to enable/disable network protocol suite statistics
 *               counters.
 *
 *           (3) Configure NET_CTR_CFG_ERR_EN  to enable/disable network protocol suite error counters.
 ********************************************************************************************************
 *******************************************************************************************************/

// <q NET_DBG_CFG_MEM_CLR_EN> Clear memory
// <i> Enables the network protocol suite to clear internal data structure memory buffers; a convenient feature while debugging.
// <i> Default: 0
#define  NET_DBG_CFG_MEM_CLR_EN                             0

// <q NET_CTR_CFG_STAT_EN> Stats counter
// <i> Enables network protocol suite statistics counters.
// <i> Default: 0
#define  NET_CTR_CFG_STAT_EN                                0

// <q NET_CTR_CFG_ERR_EN> Error counters
// <i> Enables network protocol suite error counters.
// <i> Default: 0
#define  NET_CTR_CFG_ERR_EN                                 0

/********************************************************************************************************
 ********************************************************************************************************
 *                                 NETWORK STATISTIC POOL CONFIGURATION
 ********************************************************************************************************
 *******************************************************************************************************/

// <q NET_STAT_POOL_BUF_EN> Buffers statistics
// <i> Allocates code and data space for the Buffers statistic pool.
// <i> Default: 0
#define  NET_STAT_POOL_BUF_EN                               0

// <q NET_STAT_POOL_ARP_EN> ARP cache statistics
// <i> Allocates code and data space for the ARP cache statistic pool.
// <i> Default: 0
#define  NET_STAT_POOL_ARP_EN                               0

// <q NET_STAT_POOL_NDP_EN> NDP cache statistics
// <i> Allocates code and data space for the NDP cache statistic pool.
// <i> Default: 0
#define  NET_STAT_POOL_NDP_EN                               0

// <q NET_STAT_POOL_IGMP_EN> IGMP host group statistics
// <i> Allocates code and data space for the IGMP host group statistic pool.
// <i> Default: 0
#define  NET_STAT_POOL_IGMP_EN                              0

// <q NET_STAT_POOL_MLDP_EN> MLDP host group statistics
// <i> Allocates code and data space for the MLDP host group statistic pool.
// <i> Default: 0
#define  NET_STAT_POOL_MLDP_EN                              0

// <q NET_STAT_POOL_TMR_EN> Network timer statistics
// <i> Allocates code and data space for the Network Timer statistic pool.
// <i> Default: 0
#define  NET_STAT_POOL_TMR_EN                               0

// <q NET_STAT_POOL_SOCK_EN> Socket statistics
// <i> Allocates code and data space for the Socket statistic pool.
// <i> Default: 0
#define  NET_STAT_POOL_SOCK_EN                              0

// <q NET_STAT_POOL_CONN_EN> Connection statistics
// <i> Allocates code and data space for the Connection statistic pool.
// <i> Default: 0
#define  NET_STAT_POOL_CONN_EN                              0

// <q NET_STAT_POOL_TCP_CONN_EN> TCP connection statistics
// <i> Allocates code and data space for the TCP Connection statistic pool.
// <i> Default: 0
#define  NET_STAT_POOL_TCP_CONN_EN                          0

/********************************************************************************************************
 ********************************************************************************************************
 *                                NETWORK INTERFACE LAYER CONFIGURATION
 ********************************************************************************************************
 *******************************************************************************************************/

// <o NET_IF_CFG_MAX_NBR_IF> Maximum number of interfaces
// <i> Specifies the maximum number of network interfaces that the network module can create.
// <i> Default: 2
#define  NET_IF_CFG_MAX_NBR_IF                              2

// <o NET_IF_CFG_TX_SUSPEND_TIMEOUT_MS> Network interface transmit suspend timeout (milliseconds)
// <i> Default: 1
#define  NET_IF_CFG_TX_SUSPEND_TIMEOUT_MS                   1u

// <q NET_IF_CFG_LOOPBACK_EN> Loopback Interface
// <i> Allocates code and data space used to support the loopback interface for internal-only communication.
// <i> Default: 0
#define  NET_IF_CFG_LOOPBACK_EN                             0

// <q NET_IF_CFG_WAIT_SETUP_READY_EN> Wait setup ready
// <i> Specifies whether the wait feature for the interface setup is enabled
// <i> When enabled, your application can wait for the network interface setup to complete by using the function NetIF_WaitSetupReady(). The network interface setup consists mainly of IP address configurations. The NetIF_WaitSetupReady() function will wait for all configuration processes that were specified in the start function to complete before returning.
// <i> Default: 1
#define  NET_IF_CFG_WAIT_SETUP_READY_EN                     1

/********************************************************************************************************
 ********************************************************************************************************
 *                                NETWORK INTERFACE LAYER CONFIGURATION
 *
 * Note(s) : (1) Address resolution protocol ONLY required for IPv4.
 ********************************************************************************************************
 *******************************************************************************************************/

// <o NET_ARP_CFG_CACHE_NBR> Number of ARP cache entries
// <i> Default: 5
#define  NET_ARP_CFG_CACHE_NBR                              5u

/********************************************************************************************************
 ********************************************************************************************************
 *                           NEIGHBOR DISCOVERY PROTOCOL LAYER CONFIGURATION
 *
 * Note(s) : (1) Neighbor Discovery Protocol ONLY required for IPv6.
 ********************************************************************************************************
 *******************************************************************************************************/

// <o NET_NDP_CFG_CACHE_NBR> Number of NDP Neighbor cache entries
// <i> Default: 5
#define  NET_NDP_CFG_CACHE_NBR                              5u

// <o NET_NDP_CFG_DEST_NBR> Number of NDP Destination cache entries
// <i> Default: 5
#define  NET_NDP_CFG_DEST_NBR                               5u

// <o NET_NDP_CFG_PREFIX_NBR> Number of NDP Prefix entries
// <i> Default: 5
#define  NET_NDP_CFG_PREFIX_NBR                             5u

// <o NET_NDP_CFG_ROUTER_NBR> Number of NDP Router entries
// <i> Default: 3
#define  NET_NDP_CFG_ROUTER_NBR                             3u

/********************************************************************************************************
 ********************************************************************************************************
 *                            INTERNET PROTOCOL LAYER VERSION CONFIGURATION
 ********************************************************************************************************
 *******************************************************************************************************/

// <q NET_IPv4_CFG_EN> IPv4 Module
// <i> Enables the IPv4 module.
// <i> Default: 1
#define  NET_IPv4_CFG_EN                                    1

// <q NET_IPv4_CFG_LINK_LOCAL_EN> IPv4 - Link Local module
// <i> Enables the IPv4 Link-Local address module.
// <i> Default: 1
#define  NET_IPv4_CFG_LINK_LOCAL_EN                         1

// <o NET_IPv4_CFG_IF_MAX_NBR_ADDR> IPv4 - Maximum number of IPv4 addresses
// <i> maximum number of IPv4 addresses that may be configured for all network interfaces at run-time.
// <i> Default: 2
#define  NET_IPv4_CFG_IF_MAX_NBR_ADDR                       2u

// <q NET_IPv6_CFG_EN> IPv6 Module
// <i> Enables the IPv6 module.
// <i> Default: 1
#define  NET_IPv6_CFG_EN                                    1

// <q NET_IPv6_CFG_ADDR_AUTO_CFG_EN> IPv6 - SLAAC
// <i> Enables the IPv6 Stateless Address Auto-Configuration module.
// <i> Default: 1
#define  NET_IPv6_CFG_ADDR_AUTO_CFG_EN                      1

// <q NET_IPv6_CFG_DAD_EN> IPv6 - DAD
// <i> Enables the Duplication Address Detection (DAD) module.
// <i> Default: 1
#define  NET_IPv6_CFG_DAD_EN                                1

// <o NET_IPv6_CFG_IF_MAX_NBR_ADDR> IPv6 - Maximum number of IPv6 addresses
// <i> Maximum number of IPv6 addresses that may be configured for all network interfaces at run-time.
// <i> Default: 4
#define  NET_IPv6_CFG_IF_MAX_NBR_ADDR                       4u

/********************************************************************************************************
 ********************************************************************************************************
 *                  INTERNET GROUP MANAGEMENT PROTOCOL(MULTICAST) LAYER CONFIGURATION
 ********************************************************************************************************
 *******************************************************************************************************/

// <q NET_MCAST_CFG_IPv4_RX_EN> Multicast reception IPv4
// <i> Enables multicast support in reception for IPv4.
// <i> Default: 1
#define  NET_MCAST_CFG_IPv4_RX_EN                           1

// <q NET_MCAST_CFG_IPv4_TX_EN> Multicast transmission IPv4
// <i> Enables multicast support in transmission for IPv4.
// <i> Default: 1
#define  NET_MCAST_CFG_IPv4_TX_EN                           1

// <o NET_MCAST_CFG_HOST_GRP_NBR_MAX> Multicast - Maximum number of IGMP and MLDP host groups
// <i> Maximum number of IGMP and MLDP host groups that may be joined at any time.
// <i> Default: 4
#define  NET_MCAST_CFG_HOST_GRP_NBR_MAX                     4u

/********************************************************************************************************
 ********************************************************************************************************
 *                                  NETWORK SOCKET LAYER CONFIGURATION
 *
 * Note(s) : (1) The maximum accept queue size represents the number of connection that can be queued by
 *               the stack before being accepted. For a TCP server when a connection is queued, it means
 *               that the SYN, ACK packet has been sent back, so the remote host can start transmitting
 *               data once the connection is queued and the stack will queue up all data received until
 *               the connection is accepted and the data is read.
 *
 *           (2) Receive and transmit queue size MUST be properly configured to optimize performance.
 *
 *               (a) It represents the number of bytes that can be queued by one socket. It's important
 *                   that all socket are not able to queue more data than what the device can hold in its
 *                   buffers.
 *
 *               (b) The size should be also a multiple of the maximum segment size (MSS) to optimize
 *                   performance. UDP MSS is 1470 and TCP MSS is 1460.
 *
 *               (c) RX and TX queue size can be reduce at runtime using socket option API.
 *
 *               (d) Window calculation example:
 *                       Number of TCP connection  : 2
 *                       Number of UDP connection  : 0
 *                       Number of RX large buffer : 10
 *                       Number of TX Large buffer : 6
 *                       Number of TX small buffer : 2
 *                       Size of RX large buffer   : 1518
 *                       Size of TX large buffer   : 1518
 *                       Size of TX small buffer   : 60
 *                       TCP MSS RX                = 1460
 *                       TCP MSS TX large buffer   = 1460
 *                       TCP MSS TX small buffer   = 0
 *                       Maximum receive  window   = (10 * 1460)           = 14600 bytes
 *                       Maximum transmit window   = (6  * 1460) + (2 * 0) = 8760  bytes
 *                       RX window size per socket = (14600 / 2)           =  7300 bytes
 *                       TX window size per socket = (8760  / 2)           =  4380 bytes
 ********************************************************************************************************
 *******************************************************************************************************/

// <o NET_SOCK_CFG_SOCK_NBR_UDP> Sockets - Maximum number of UDP connections
// <i> Default: 2
#define  NET_SOCK_CFG_SOCK_NBR_UDP                          2u

// <o NET_SOCK_CFG_SOCK_NBR_TCP> Sockets - Maximum number of TCP connections
// <i> Default: 5
#define  NET_SOCK_CFG_SOCK_NBR_TCP                          5u

// <q NET_SOCK_CFG_SEL_EN> Sockets - Socket Select
// <i> Enables socket select functionality.
// <i> Default: 1
#define  NET_SOCK_CFG_SEL_EN                                1

// <o NET_SOCK_CFG_CONN_ACCEPT_Q_SIZE_MAX> Sockets - Maximum size of the stream-type sockets' accept queue
// <i> Default: 2
#define  NET_SOCK_CFG_CONN_ACCEPT_Q_SIZE_MAX                2u

// <o NET_SOCK_CFG_RX_Q_SIZE_OCTET> Sockets - Buffer size of the socket receive queue
// <i> Default: 4096
#define  NET_SOCK_CFG_RX_Q_SIZE_OCTET                       4096u

// <o NET_SOCK_CFG_TX_Q_SIZE_OCTET> Sockets - Buffer size of the socket transmit queue
// <i> Default: 4096
#define  NET_SOCK_CFG_TX_Q_SIZE_OCTET                       4096u

/********************************************************************************************************
 ********************************************************************************************************
 *                          TRANSMISSION CONTROL PROTOCOL LAYER CONFIGURATION
 ********************************************************************************************************
 *******************************************************************************************************/

// <q NET_TCP_CFG_EN> TCP Module
// <i> Enables the TCP module.
// <i> Default: 1
#define  NET_TCP_CFG_EN                                     1

/********************************************************************************************************
 ********************************************************************************************************
 *                              USER DATAGRAM PROTOCOL LAYER CONFIGURATION
 *
 * Note(s) : (1) Configure NET_UDP_CFG_RX_CHK_SUM_DISCARD_EN to enable/disable discarding of UDP
 *               datagrams received with NO computed checksum:
 *                   When ENABLED,  ALL UDP datagrams received without a checksum are discarded.
 *                   When DISABLED, ALL UDP datagrams received without a checksum are flagged so that
 *                   application(s) may handle &/or discard.
 *
 *           (2) Configure NET_UDP_CFG_TX_CHK_SUM_EN to enable/disable transmitting UDP datagrams with
 *               checksums :
 *                   When ENABLED,  ALL UDP datagrams are transmitted with a computed checksum.
 *                   When DISABLED, ALL UDP datagrams are transmitted without a computed checksum.
 ********************************************************************************************************
 *******************************************************************************************************/

// <q NET_UDP_CFG_RX_CHK_SUM_DISCARD_EN> UDP RX Checksum Discard
// <i> Determines whether received UDP packets without a valid checksum are discarded or are handled and processed.
// <i> Default: 0
#define  NET_UDP_CFG_RX_CHK_SUM_DISCARD_EN                  0

// <q NET_UDP_CFG_TX_CHK_SUM_EN> UDP TX Checksum Compute
// <i> Determine whether UDP checksums are computed for transmission to other hosts.
// <i> Default: 0
#define  NET_UDP_CFG_TX_CHK_SUM_EN                          0

/********************************************************************************************************
 ********************************************************************************************************
 *                           NETWORK SECURITY MANAGER CONFIGURATION (SSL/TLS)
 *
 * Note(s) : (1) The network security layer can be enabled ONLY if the application project contains a
 *               secure module supported by Micrium OS Net such as:
 *                   - wolfSSL (formerly CyaSSL) provided by wolfSSL.
 *                   - mbed TLS.
 ********************************************************************************************************
 *******************************************************************************************************/

// <o NET_SECURE_CFG_MAX_NBR_SOCK_SERVER> Security - Maximum number of server secure sockets
// <i> The sum of NET_SECURE_CFG_MAX_NBR_SOCK_SERVER and NET_SECURE_CFG_MAX_NBR_SOCK_CLIENT must not be greater than NET_SOCK_CFG_SOCK_NBR_TCP.
// <i> Default: 2
#define  NET_SECURE_CFG_MAX_NBR_SOCK_SERVER                 2u

// <o NET_SECURE_CFG_MAX_NBR_SOCK_CLIENT> Security - Maximum number of client secure sockets
// <i> The sum of NET_SECURE_CFG_MAX_NBR_SOCK_SERVER and NET_SECURE_CFG_MAX_NBR_SOCK_CLIENT must not be greater than NET_SOCK_CFG_SOCK_NBR_TCP.
// <i> Default: 2
#define  NET_SECURE_CFG_MAX_NBR_SOCK_CLIENT                 2u

// <o NET_SECURE_CFG_MAX_CERT_LEN> Security - Maximum length (in octets) of server certificates
// <i> Default: 2048
#define  NET_SECURE_CFG_MAX_CERT_LEN                        2048u

// <o NET_SECURE_CFG_MAX_KEY_LEN> Security - Maximum length (in octets) of server keys
// <i> Default: 2048
#define  NET_SECURE_CFG_MAX_KEY_LEN                         2048u

// <o NET_SECURE_CFG_MAX_NBR_CA> Security - Maximum number of certificate authorities that can be installed
// <i> Default: 1
#define  NET_SECURE_CFG_MAX_NBR_CA                          1u

// <o NET_SECURE_CFG_MAX_CA_CERT_LEN> Security - Maximum length (in octets) of certificate authority certificates
// <i> Default: 2048
#define  NET_SECURE_CFG_MAX_CA_CERT_LEN                     2048u

/********************************************************************************************************
 ********************************************************************************************************
 *                                    NETWORK MODULES CONFIGURATION
 ********************************************************************************************************
 *******************************************************************************************************/

// <q NET_DHCP_CLIENT_CFG_MODULE_EN> DHCP Client
// <i> Enables the DHCP client module.
// <i> Default: 1
#define  NET_DHCP_CLIENT_CFG_MODULE_EN                      1

// <q NET_DNS_CLIENT_CFG_MODULE_EN> DNS Client
// <i> Enables the DNS client module.
// <i> Default: 1
#define  NET_DNS_CLIENT_CFG_MODULE_EN                       1

/********************************************************************************************************
 ********************************************************************************************************
 *                              INTERFACE CHECKSUM OFFLOAD CONFIGURATION
 *
 * Note(s) : (1) A checksum offload configuration can be enabled only if all your interfaces support
 *               the specific checksum offload option.
 *
 *           (2) By default, a driver should enable all checksum offload options it supports.
 ********************************************************************************************************
 *******************************************************************************************************/

// <q NET_IPV4_CFG_CHK_SUM_OFFLOAD_RX_EN> IPv4 Rx Checksum Offload
// <i> Enable to offload to the controller the validation in reception for IPv4 checksums.
// <i> Disable to have the stack do the validation in reception for IPv4 checksums.
// <i> Default: 1
#define  NET_IPV4_CFG_CHK_SUM_OFFLOAD_RX_EN                 1

// <q NET_IPV4_CFG_CHK_SUM_OFFLOAD_TX_EN> IPv4 Tx Checksum Offload
// <i> Enable to offload to the controller the validation in transmission for IPv4 checksums.
// <i> Disable to have the stack do the validation in reception for IPv4 checksums.
// <i> Default: 1
#define  NET_IPV4_CFG_CHK_SUM_OFFLOAD_TX_EN                 1

// <q NET_ICMP_CFG_CHK_SUM_OFFLOAD_RX_EN> ICMP Rx Checksum Offload
// <i> Enable to offload to the controller the validation in reception for ICMP checksums.
// <i> Disable to have the stack do the validation in reception for ICMP checksums.
// <i> Default: 0
#define  NET_ICMP_CFG_CHK_SUM_OFFLOAD_RX_EN                 0

// <q NET_ICMP_CFG_CHK_SUM_OFFLOAD_TX_EN> ICMP Tx Checksum Offload
// <i> Enable to offload to the controller the validation in transmission for ICMP checksums.
// <i> Disable to have the stack do the validation in reception for ICMP checksums.
// <i> Default: 0
#define  NET_ICMP_CFG_CHK_SUM_OFFLOAD_TX_EN                 0

// <q NET_UDP_CFG_CHK_SUM_OFFLOAD_RX_EN> UDP Rx Checksum Offload
// <i> Enable to offload to the controller the validation in reception for UDP checksums.
// <i> Disable to have the stack do the validation in reception for UDP checksums.
// <i> Default: 1
#define  NET_UDP_CFG_CHK_SUM_OFFLOAD_RX_EN                  1

// <q NET_UDP_CFG_CHK_SUM_OFFLOAD_TX_EN> UDP Tx Checksum Offload
// <i> Enable to offload to the controller the validation in transmission for UDP checksums.
// <i> Disable to have the stack do the validation in reception for UDP checksums.
// <i> Default: 1
#define  NET_UDP_CFG_CHK_SUM_OFFLOAD_TX_EN                  1

// <q NET_TCP_CFG_CHK_SUM_OFFLOAD_RX_EN> TCP Rx Checksum Offload
// <i> Enable to offload to the controller the validation in reception for TCP checksums.
// <i> Disable to have the stack do the validation in reception for TCP checksums.
// <i> Default: 1
#define  NET_TCP_CFG_CHK_SUM_OFFLOAD_RX_EN                  1

// <q NET_TCP_CFG_CHK_SUM_OFFLOAD_TX_EN> TCP Tx Checksum Offload
// <i> Enable to offload to the controller the validation in transmission for TCP checksums.
// <i> Disable to have the stack do the validation in reception for TCP checksums.
// <i> Default: 1
#define  NET_TCP_CFG_CHK_SUM_OFFLOAD_TX_EN                  1

/********************************************************************************************************
 ********************************************************************************************************
 *                                             MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // End of net_cfg.h module include.
