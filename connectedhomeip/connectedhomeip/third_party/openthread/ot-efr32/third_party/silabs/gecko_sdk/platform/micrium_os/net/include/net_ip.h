/***************************************************************************//**
 * @file
 * @brief Network Ip Layer - (Internet Protocol)
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

#ifndef  _NET_IP_H_
#define  _NET_IP_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                            INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <net/include/net_cfg_net.h>
#include  <net/include/net_type.h>

#include  <cpu/include/cpu.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                        IP ADDRESS DATA DEFINES
 *******************************************************************************************************/

#if     (defined(NET_IPv4_MODULE_EN) && !defined(NET_IPv6_MODULE_EN))
#define  NET_IP_MAX_ADDR_SIZE                        NET_IPv4_ADDR_SIZE
#elif  (!defined(NET_IPv4_MODULE_EN) &&  defined(NET_IPv6_MODULE_EN))
#define  NET_IP_MAX_ADDR_SIZE                        NET_IPv6_ADDR_SIZE
#elif   (defined(NET_IPv4_MODULE_EN) &&  defined(NET_IPv6_MODULE_EN))
#define  NET_IP_MAX_ADDR_SIZE                        NET_IPv6_ADDR_SIZE
#else
#define  NET_IP_MAX_ADDR_SIZE                        NET_IPv6_ADDR_SIZE
#endif

#if     (defined(NET_IPv4_MODULE_EN) && !defined(NET_IPv6_MODULE_EN))
#define  NET_IP_MAX_ADDR_LEN                        NET_IPv4_ADDR_LEN
#elif  (!defined(NET_IPv4_MODULE_EN) &&  defined(NET_IPv6_MODULE_EN))
#define  NET_IP_MAX_ADDR_LEN                        NET_IPv6_ADDR_LEN
#elif   (defined(NET_IPv4_MODULE_EN) &&  defined(NET_IPv6_MODULE_EN))
#define  NET_IP_MAX_ADDR_LEN                       NET_IPv6_ADDR_LEN
#else
#define  NET_IP_MAX_ADDR_LEN                       NET_IPv6_ADDR_LEN
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                             DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                        IP ADDRESS DATA TYPES
 *******************************************************************************************************/

typedef  CPU_INT08U NET_IP_ADDR_LEN;

typedef  CPU_INT08U NET_IP_ADDRS_QTY;                           ///< Defines max qty of IP addrs per IF to support.

typedef  struct  net_ip_addr {
  CPU_INT08U Array[NET_IP_MAX_ADDR_LEN];
} NET_IP_ADDR;

typedef  struct  net_ip_addr_obj {
  NET_IP_ADDR     Addr;
  NET_IP_ADDR_LEN AddrLen;
} NET_IP_ADDR_OBJ;

typedef  struct  net_host_ip_addr NET_HOST_IP_ADDR;

struct  net_host_ip_addr {
  NET_IP_ADDR_OBJ  AddrObj;
  NET_HOST_IP_ADDR *NextPtr;
};

typedef  enum {
  NET_IP_ADDR_CFG_MODE_NONE,
  NET_IP_ADDR_CFG_MODE_DYN,
  NET_IP_ADDR_CFG_MODE_DYN_INIT,
  NET_IP_ADDR_CFG_MODE_STATIC,
  NET_IP_ADDR_CFG_MODE_AUTO_CFG
} NET_IP_ADDR_CFG_MODE;

typedef enum {
  NET_IP_ADDR_FAMILY_UNKNOWN,
  NET_IP_ADDR_FAMILY_NONE,
  NET_IP_ADDR_FAMILY_IPv4,
  NET_IP_ADDR_FAMILY_IPv6
} NET_IP_ADDR_FAMILY;

typedef  struct  net_ipv4_mreq {                                ///< IPv4 Multicast Membership Request (socket option)
  NET_IPv4_ADDR mcast_addr;                                     ///< IP Address of the multicast group
  NET_IPv4_ADDR if_ip_addr;                                     ///< IP Address of the IF on which to join the group
} NET_IPv4_MREQ;

/********************************************************************************************************
 ********************************************************************************************************
 *                                             MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // _NET_IP_H_
