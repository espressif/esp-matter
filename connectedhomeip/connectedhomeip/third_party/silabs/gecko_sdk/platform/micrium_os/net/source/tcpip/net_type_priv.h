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

#ifndef  _NET_TYPE_PRIV_H_
#define  _NET_TYPE_PRIV_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  "../../include/net_cfg_net.h"
#include  "../../include/net_type.h"

#include  <cpu/include/cpu.h>
#include  <common/source/kal/kal_priv.h>
#include  <common/source/collections/slist_priv.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DATA TYPES
 *
 * Note(s) : (1) Ideally, each network module &/or protocol layer would define all its own data types.
 *               However, some network module &/or protocol layer data types MUST be defined PRIOR to
 *               all other network modules/layers that require their definitions.
 *
 *               See also 'net.h  NETWORK INCLUDE FILES'.
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                       NETWORK TRANSACTION DATA TYPE
 *******************************************************************************************************/

typedef  enum  net_transaction {
  NET_TRANSACTION_NONE = 0,
  NET_TRANSACTION_RX = 100,
  NET_TRANSACTION_TX = 200
} NET_TRANSACTION;

/********************************************************************************************************
 *                           NETWORK MAXIMUM TRANSMISSION UNIT (MTU) DATA TYPE
 *
 * Note(s) : (1) NET_MTU_MIN_VAL & NET_MTU_MAX_VAL  SHOULD be #define'd based on 'NET_MTU' data type declared.
 *******************************************************************************************************/

//                                                                 See Note #1.
#define  NET_MTU_MIN_VAL                DEF_INT_16U_MIN_VAL
#define  NET_MTU_MAX_VAL                DEF_INT_16U_MAX_VAL

/********************************************************************************************************
 *                                       NETWORK CHECK-SUM DATA TYPE
 *******************************************************************************************************/

typedef  CPU_INT16U NET_CHK_SUM;

/********************************************************************************************************
 *                                       NETWORK BUFFER DATA TYPES
 *
 * Note(s) : (1) NET_BUF_NBR_MAX  SHOULD be #define'd based on 'NET_BUF_QTY' data type declared.
 *******************************************************************************************************/

#define  NET_BUF_NBR_MIN                                   1
#define  NET_BUF_NBR_MAX                 DEF_INT_16U_MAX_VAL    // See Note #1.

/********************************************************************************************************
 *                                   NETWORK PACKET COUNTER DATA TYPE
 *******************************************************************************************************/

#define  NET_PKT_CTR_MIN                NET_BUF_NBR_MIN
#define  NET_PKT_CTR_MAX                NET_BUF_NBR_MAX

/********************************************************************************************************
 *                                       NETWORK CONNECTION DATA TYPES
 *******************************************************************************************************/

typedef  enum  net_addr_hw_type {
  NET_ADDR_HW_TYPE_NONE,
  NET_ADDR_HW_TYPE_802x
} NET_ADDR_HW_TYPE;

/********************************************************************************************************
 *                           NETWORK TRANSPORT LAYER PORT NUMBER DATA TYPES
 *
 * Note(s) : (1) NET_PORT_NBR_MAX  SHOULD be #define'd based on 'NET_PORT_NBR' data type declared.
 *******************************************************************************************************/

#define  NET_PORT_NBR_MAX               DEF_INT_16U_MAX_VAL     // See Note #1.

/********************************************************************************************************
 *                                   NETWORK INTERFACE & DEVICE DATA TYPES
 *
 * Note(s) : (1) NET_IF_NBR_MIN_VAL & NET_IF_NBR_MAX_VAL  SHOULD be #define'd based on 'NET_IF_NBR'
 *               data type declared.
 *******************************************************************************************************/
//                                                                 See Note #1.

typedef  CPU_INT16U NET_IF_FLAG;

#define  NET_IF_FLAG_NONE               DEF_INT_16U_MIN_VAL
#define  NET_IF_FLAG_FRAG               DEF_BIT_00

typedef  NET_BUF_QTY NET_IF_Q_SIZE;                             // Defines max size of net IF q's to support.

#define  NET_IF_Q_SIZE_MIN              NET_BUF_NBR_MIN
#define  NET_IF_Q_SIZE_MAX              NET_BUF_NBR_MAX

typedef  struct  net_dev_cfg NET_DEV_CFG;

/********************************************************************************************************
 *                                       NETWORK IP LAYER DATA TYPES
 *******************************************************************************************************/

typedef  struct  net_ipv6_auto_cfg_obj NET_IPv6_AUTO_CFG_OBJ;

struct  net_ip_if_obj {
#ifdef NET_IPv4_MODULE_EN
  SLIST_MEMBER          *IPv4_AddrListPtr;
#endif
#ifdef NET_IPv6_MODULE_EN
  SLIST_MEMBER          *IPv6_AddrListPtr;
  NET_IPv6_AUTO_CFG_OBJ *IPv6_AutoCfgObjPtr;
  KAL_SEM_HANDLE        IPv6_NDP_RxRouterAdvSignal;
#endif
};

/********************************************************************************************************
 *                                       NETWORK IPv4 LAYER DATA TYPES
 *******************************************************************************************************/

typedef  CPU_INT16U NET_IPv4_HDR_FLAGS;

/********************************************************************************************************
 *                                       NETWORK IPv6 LAYER DATA TYPES
 *******************************************************************************************************/

typedef  NET_FLAGS NET_IPv6_FLAGS;

typedef  CPU_INT16U NET_IPv6_FRAG_FLAGS;

/********************************************************************************************************
 *                                       NETWORK TCP LAYER DATA TYPES
 *******************************************************************************************************/

/********************************************************************************************************
 *                                       TCP SEQUENCE NUMBER DATA TYPE
 *
 * Note(s) : (1) 'NET_TCP_SEQ_NBR'  pre-defined in 'net_type.h' PRIOR to all other network modules that
 *                   require TCP sequence number data type(s).
 *******************************************************************************************************/

//                                                                 See Note #1.
typedef  CPU_INT32U NET_TCP_SEQ_NBR;

/********************************************************************************************************
 *                                       TCP SEGMENT SIZE DATA TYPE
 *
 * Note(s) : (1) 'NET_TCP_SEG_SIZE' pre-defined in 'net_type.h' PRIOR to all other network modules that
 *                   require TCP segment size data type(s).
 *******************************************************************************************************/

typedef  NET_FLAGS NET_TCP_FLAGS;
typedef  CPU_INT16U NET_TCP_HDR_FLAGS;

/********************************************************************************************************
 *                                   TCP RTT MEASUREMENT DATA TYPES
 *
 * Note(s) : (1) RTT measurement data types MUST be defined to ensure sufficient range for both scaled
 *               & un-scaled, signed & unsigned time measurement values.
 *
 *           (2) 'NET_TCP_TX_RTT_TS_MS' pre-defined in 'net_type.h' PRIOR to all other network modules
 *                   that require TCP Transmit Round-Trip Time data type(s).
 *******************************************************************************************************/
//                                                                 See Note #2.
typedef  NET_TS_MS NET_TCP_TX_RTT_TS_MS;

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // _NET_TYPE_PRIV_H_
