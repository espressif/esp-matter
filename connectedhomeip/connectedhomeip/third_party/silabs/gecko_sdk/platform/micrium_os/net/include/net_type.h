/***************************************************************************//**
 * @file
 * @brief Network Data Types
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

#ifndef  _NET_TYPE_H_
#define  _NET_TYPE_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                            INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <cpu/include/cpu.h>
#include  <common/include/lib_def.h>
#include  <common/include/rtos_err.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                             DATA TYPES
 *
 * Note(s) : (1) Ideally, each network module &/or protocol layer would define all its own data types.
 *               However, some network module &/or protocol layer data types MUST be defined PRIOR to
 *               all other network modules/layers that require their definitions.
 *
 *               See also 'net.h  NETWORK INCLUDE FILES'.
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                      NETWORK COUNTER DATA TYPE
 *******************************************************************************************************/

typedef  CPU_INT32U NET_CTR;                                    ///< Defines   max nbr of errs/stats to cnt.

/********************************************************************************************************
 *                                    NETWORK TIMESTAMP DATA TYPES
 *
 * Note(s) : (1) RFC #791, Section 3.1 'Options : Internet Timestamp' states that "the Timestamp is a
 *               right-justified, 32-bit timestamp in milliseconds since midnight UT [Universal Time]".
 *******************************************************************************************************/

typedef  CPU_INT32U NET_TS;
typedef  NET_TS NET_TS_MS;

/********************************************************************************************************
 *                                       NETWORK FLAGS DATA TYPE
 *******************************************************************************************************/

typedef  CPU_INT16U NET_FLAGS;

/********************************************************************************************************
 *                           NETWORK TRANSPORT LAYER PORT NUMBER DATA TYPES
 *
 * Note(s) : (1) NET_PORT_NBR_MAX  SHOULD be #define'd based on 'NET_PORT_NBR' data type declared.
 *******************************************************************************************************/

typedef  CPU_INT16U NET_PORT_NBR;
typedef  CPU_INT16U NET_PORT_NBR_QTY;                           ///< Defines max qty of port nbrs to support.

/********************************************************************************************************
 *                          NETWORK MAXIMUM TRANSMISSION UNIT (MTU) DATA TYPE
 *
 * Note(s) : (1) NET_MTU_MIN_VAL & NET_MTU_MAX_VAL  SHOULD be #define'd based on 'NET_MTU' data type declared.
 *******************************************************************************************************/

typedef  CPU_INT16U NET_MTU;

/********************************************************************************************************
 *                                NETWORK INTERFACE & DEVICE DATA TYPES
 *
 * Note(s) : (1) NET_IF_NBR_MIN_VAL & NET_IF_NBR_MAX_VAL  SHOULD be #define'd based on 'NET_IF_NBR'
 *               data type declared.
 *******************************************************************************************************/

typedef  CPU_INT08U NET_IF_NBR;

/********************************************************************************************************
 *                                      NETWORK IPv4 DATA TYPES
 *******************************************************************************************************/

typedef  CPU_INT32U NET_IPv4_ADDR;                              ///< Defines IPv4 IP addr size.

#define  NET_IPv4_ADDR_LEN                       (sizeof(NET_IPv4_ADDR))
#define  NET_IPv4_ADDR_SIZE                      (sizeof(NET_IPv4_ADDR))

typedef  CPU_INT08U NET_IPv4_TOS;
typedef  CPU_INT08U NET_IPv4_TTL;

typedef  NET_FLAGS NET_IPv4_FLAGS;

//                                                                 'This Host' IPv4 init addr (see Note #3a1).
#define  NET_IPv4_ADDR_THIS_HOST          (((NET_IPv4_ADDR)  0uL << (3u * DEF_OCTET_NBR_BITS))   \
                                           | ((NET_IPv4_ADDR)  0uL << (2u * DEF_OCTET_NBR_BITS)) \
                                           | ((NET_IPv4_ADDR)  0uL << (1u * DEF_OCTET_NBR_BITS)) \
                                           | ((NET_IPv4_ADDR)  0uL << (0u * DEF_OCTET_NBR_BITS)))

#define  NET_IPv4_ADDR_NONE                  NET_IPv4_ADDR_THIS_HOST
#define  NET_IPv4_ADDR_ANY                   NET_IPv4_ADDR_NONE

//                                                                 Limited broadcast addr   (see Note #3e1).
#define  NET_IPv4_ADDR_BROADCAST           (((NET_IPv4_ADDR)255uL << (3u * DEF_OCTET_NBR_BITS))   \
                                            | ((NET_IPv4_ADDR)255uL << (2u * DEF_OCTET_NBR_BITS)) \
                                            | ((NET_IPv4_ADDR)255uL << (1u * DEF_OCTET_NBR_BITS)) \
                                            | ((NET_IPv4_ADDR)255uL << (0u * DEF_OCTET_NBR_BITS)))

typedef  enum  net_ipv4_link_local_status {
  NET_IPv4_LINK_LOCAL_STATUS_NONE,
  NET_IPv4_LINK_LOCAL_STATUS_SUCCEEDED,
  NET_IPv4_LINK_LOCAL_STATUS_FAILED
} NET_IPv4_LINK_LOCAL_STATUS;

typedef  void (*NET_IPv4_LINK_LOCAL_COMPLETE_HOOK) (NET_IF_NBR                 if_nbr,
                                                    NET_IPv4_ADDR              link_local_addr,
                                                    NET_IPv4_LINK_LOCAL_STATUS status,
                                                    RTOS_ERR                   err);

/********************************************************************************************************
 *                                      NETWORK IPv6 DATA TYPES
 *******************************************************************************************************/

#define  NET_IPv6_ADDR_LEN             (4 * sizeof(CPU_INT32U))

typedef  struct  net_ipv6_addr {                                ///< Defines IPv6 IP addr size.
  CPU_INT08U Addr[NET_IPv6_ADDR_LEN];
} NET_IPv6_ADDR;

#define  NET_IPv6_ADDR_SIZE            (sizeof(NET_IPv6_ADDR))
#define  NET_IPv6_ADDR_LEN_NBR_BITS     NET_IPv6_ADDR_LEN * DEF_OCTET_NBR_BITS

typedef  CPU_INT08U NET_IPv6_ADDR_TYPE;
typedef  CPU_INT16U NET_IPv6_TRAFFIC_CLASS;
typedef  CPU_INT08U NET_IPv6_HOP_LIM;
typedef  CPU_INT32U NET_IPv6_FLOW_LABEL;

#define  NET_IPv6_ADDR_ANY_INIT                    { { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } }
#define  NET_IPv6_ADDR_LOOPBACK_INIT               { { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 } }
#define  NET_IPv6_ADDR_LINKLOCAL_ALLNODES_INIT     { { 0xFF, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 } }
#define  NET_IPv6_ADDR_LINKLOCAL_ALLROUTERS_INIT   { { 0xFF, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2 } }

#define  NET_IPv6_ADDR_ANY                               NET_IPv6_ADDR_ANY_INIT
#define  NET_IPv6_ADDR_NONE                              NET_IPv6_ADDR_ANY
#define  NET_IPv6_ADDR_WILDCARD                          NET_IPv6_ADDR_ANY
#define  NET_IPv6_ADDR_LOOPBACK                          NET_IPv6_ADDR_LOOPBACK_INIT
#define  NET_IPv6_ADDR_LINKLOCAL_ALLNODES                NET_IPv6_ADDR_LINKLOCAL_ALLNODES_INIT
#define  NET_IPv6_ADDR_LINKLOCAL_ALLROUTERS              NET_IPv6_ADDR_LINKLOCAL_ALLROUTERS_INIT

typedef enum {
  NET_IPv6_CFG_ADDR_TYPE_STATIC,
  NET_IPv6_CFG_ADDR_TYPE_AUTO_CFG_LINK_LOCAL,
  NET_IPv6_CFG_ADDR_TYPE_AUTO_CFG_GLOBAL
} NET_IPv6_CFG_ADDR_TYPE;

typedef enum net_ipv6_addr_cfg_status {
  NET_IPv6_ADDR_CFG_STATUS_NONE,
  NET_IPv6_ADDR_CFG_STATUS_NOT_DONE,
  NET_IPv6_ADDR_CFG_STATUS_IN_PROGRESS,
  NET_IPv6_ADDR_CFG_STATUS_SUCCEED,
  NET_IPv6_ADDR_CFG_STATUS_FAIL,
  NET_IPv6_ADDR_CFG_STATUS_DUPLICATE
} NET_IPv6_ADDR_CFG_STATUS;

typedef  void (*NET_IPv6_ADDR_HOOK_FNCT)(NET_IF_NBR               if_nbr,
                                         NET_IPv6_CFG_ADDR_TYPE   addr_type,
                                         const NET_IPv6_ADDR      *p_addr_cfgd,
                                         NET_IPv6_ADDR_CFG_STATUS addr_cfg_status);

/********************************************************************************************************
 *                                    NETWORK CONNECTION DATA TYPES
 *******************************************************************************************************/

typedef  CPU_INT16S NET_CONN_ID;

/********************************************************************************************************
 *                                      TCP WINDOW SIZE DATA TYPE
 *
 * Note(s) : (1) 'NET_TCP_WIN_SIZE' pre-defined in 'net_type.h' PRIOR to all other network modules that
 *                require TCP window size data type(s).
 *******************************************************************************************************/
// See Note #1.
typedef  CPU_INT16U NET_TCP_WIN_SIZE;

/********************************************************************************************************
 *                                     TCP SEGMENT SIZE DATA TYPE
 *
 * Note(s) : (1) 'NET_TCP_SEG_SIZE' pre-defined in 'net_type.h' PRIOR to all other network modules that
 *                require TCP segment size data type(s).
 *******************************************************************************************************/

// See Note #1.
typedef  CPU_INT16U NET_TCP_SEG_SIZE;

/********************************************************************************************************
 *                               NETWORK CACHE ADDRESS LENGTH DATA TYPE
 *******************************************************************************************************/

typedef  CPU_INT08U NET_CACHE_ADDR_LEN;

/********************************************************************************************************
 *                                    NETWORK BUFFER SIZE DATA TYPE
 *
 * Note(s) : (1) NET_BUF_IX_NONE  SHOULD be #define'd based on 'NET_BUF_SIZE' data type declared.
 *******************************************************************************************************/

typedef  CPU_INT16U NET_BUF_SIZE;                               ///< Defines   max size of net buf data.

#define  NET_BUF_IX_NONE                 DEF_INT_16U_MAX_VAL    ///< Define as max unsigned val (see Note #1).

/********************************************************************************************************
 *                                      NETWORK BUFFER DATA TYPES
 *
 * Note(s) : (1) NET_BUF_NBR_MAX  SHOULD be #define'd based on 'NET_BUF_QTY' data type declared.
 *******************************************************************************************************/

typedef  CPU_INT16U NET_BUF_QTY;                                ///< Defines max qty of net bufs to support.

typedef  struct  net_buf NET_BUF;

/********************************************************************************************************
 *                                  NETWORK PACKET COUNTER DATA TYPE
 *******************************************************************************************************/

typedef  NET_BUF_QTY NET_PKT_CTR;                               ///< Defines max nbr of pkts to cnt.

/********************************************************************************************************
 *                                     NETWORK PROTOCOL DATA TYPE
 *******************************************************************************************************/

typedef  enum  net_protocol_type {
  //                                                               ---------------- NET PROTOCOL TYPES ----------------
  NET_PROTOCOL_TYPE_NONE = 0,
  NET_PROTOCOL_TYPE_ALL = 1,

  //                                                               --------------- LINK LAYER PROTOCOLS ---------------
  NET_PROTOCOL_TYPE_LINK = 10,

  //                                                               -------------- NET IF LAYER PROTOCOLS --------------
  NET_PROTOCOL_TYPE_IF = 20,
  NET_PROTOCOL_TYPE_IF_FRAME = 21,

  NET_PROTOCOL_TYPE_IF_ETHER = 25,
  NET_PROTOCOL_TYPE_IF_IEEE_802 = 26,

  NET_PROTOCOL_TYPE_ARP = 30,
  NET_PROTOCOL_TYPE_NDP = 35,

  //                                                               ---------------- NET LAYER PROTOCOLS ---------------
  NET_PROTOCOL_TYPE_IP_V4 = 40,
  NET_PROTOCOL_TYPE_IP_V4_OPT = 41,

  NET_PROTOCOL_TYPE_IP_V6 = 42,
  NET_PROTOCOL_TYPE_IP_V6_EXT_HOP_BY_HOP = 43,
  NET_PROTOCOL_TYPE_IP_V6_EXT_ROUTING = 44,
  NET_PROTOCOL_TYPE_IP_V6_EXT_FRAG = 45,
  NET_PROTOCOL_TYPE_IP_V6_EXT_ESP = 46,
  NET_PROTOCOL_TYPE_IP_V6_EXT_AUTH = 47,
  NET_PROTOCOL_TYPE_IP_V6_EXT_NONE = 48,
  NET_PROTOCOL_TYPE_IP_V6_EXT_DEST = 49,
  NET_PROTOCOL_TYPE_IP_V6_EXT_MOBILITY = 50,

  NET_PROTOCOL_TYPE_ICMP_V4 = 60,
  NET_PROTOCOL_TYPE_ICMP_V6 = 61,

  NET_PROTOCOL_TYPE_IGMP = 62,

  //                                                               ------------- TRANSPORT LAYER PROTOCOLS ------------
  NET_PROTOCOL_TYPE_UDP_V4 = 70,
  NET_PROTOCOL_TYPE_TCP_V4 = 71,

  NET_PROTOCOL_TYPE_UDP_V6 = 72,
  NET_PROTOCOL_TYPE_TCP_V6 = 73,

  //                                                               ---------------- APP LAYER PROTOCOLS ---------------
  NET_PROTOCOL_TYPE_APP = 80,
  NET_PROTOCOL_TYPE_SOCK = 81
} NET_PROTOCOL_TYPE;

/********************************************************************************************************
 ********************************************************************************************************
 *                                             MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // _NET_TYPE_H_
