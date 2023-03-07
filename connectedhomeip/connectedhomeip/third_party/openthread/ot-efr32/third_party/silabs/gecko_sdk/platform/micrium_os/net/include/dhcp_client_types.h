/***************************************************************************//**
 * @file
 * @brief Network - Dhcp Client Types
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

#ifndef  _DHCP_CLIENT_TYPES_H_
#define  _DHCP_CLIENT_TYPES_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                            INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <net/include/net_type.h>

#include  <cpu/include/cpu.h>
#include  <common/include/lib_def.h>
#include  <common/include/rtos_err.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                    DEFAULT CONFIGUATION DEFINES
 *******************************************************************************************************/

#define  DHCPc_CFG_PORT_SERVER_DFLT                     67
#define  DHCPc_CFG_PORT_CLIENT_DFLT                     68

#define  DHCPc_CFG_TX_RETRY_NBR_DFLT                    2u
#define  DHCPc_CFG_TX_WAIT_TIMEOUT_MS_DFLT             (3u * DEF_TIME_NBR_mS_PER_SEC)

#define  DHCPc_CFG_DFLT     {                          \
    .ServerPortNbr = DHCPc_CFG_PORT_SERVER_DFLT,       \
    .ClientPortNbr = DHCPc_CFG_PORT_CLIENT_DFLT,       \
    .TxRetryNbr = DHCPc_CFG_TX_RETRY_NBR_DFLT,         \
    .TxTimeout_ms = DHCPc_CFG_TX_WAIT_TIMEOUT_MS_DFLT, \
    .ValidateAddr = DEF_NO,                            \
}

#define  DHCPc_CFG_DFLT_INIT(cfg) {                       \
    cfg.ServerPortNbr = DHCPc_CFG_PORT_SERVER_DFLT;       \
    cfg.ClientPortNbr = DHCPc_CFG_PORT_CLIENT_DFLT;       \
    cfg.TxRetryNbr = DHCPc_CFG_TX_RETRY_NBR_DFLT;         \
    cfg.TxTimeout_ms = DHCPc_CFG_TX_WAIT_TIMEOUT_MS_DFLT; \
    cfg.ValidateAddr = DEF_NO;                            \
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                             DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                       RESULT STATUS DATA TYPE
 * @brief DHCP Client status
 *******************************************************************************************************/

typedef  enum  dhcpc_status {
  DHCPc_STATUS_NONE,
  DHCPc_STATUS_SUCCESS,
  DHCPc_STATUS_LINK_LOCAL,
  DHCPc_STATUS_IN_PROGRESS,
  DHCPc_STATUS_FAIL_ADDR_USED,
  DHCPc_STATUS_FAIL_OFFER_DECLINE,
  DHCPc_STATUS_FAIL_NAK_RX,
  DHCPc_STATUS_FAIL_NO_SERVER,
  DHCPc_STATUS_FAIL_ERR_FAULT
} DHCPc_STATUS;

/********************************************************************************************************
 *                                    HOOK FUNCTIONS DATA TYPE
 *******************************************************************************************************/

typedef  void (*DHCPc_ON_COMPLETE_HOOK) (NET_IF_NBR    if_nbr,
                                         DHCPc_STATUS  status,
                                         NET_IPv4_ADDR addr,
                                         NET_IPv4_ADDR mask,
                                         NET_IPv4_ADDR gateway,
                                         RTOS_ERR      err);

/********************************************************************************************************
 *                                   RUN-TIME CONFIGURATION DATA TYPE
 *******************************************************************************************************/

typedef  struct  dhcpc_cfg {
  CPU_INT16U  ServerPortNbr;
  CPU_INT16U  ClientPortNbr;

  CPU_INT08U  TxRetryNbr;
  CPU_INT32U  TxTimeout_ms;

  CPU_BOOLEAN ValidateAddr;
} DHCPc_CFG;

/********************************************************************************************************
 ********************************************************************************************************
 *                                          GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif

extern const DHCPc_CFG DHCPc_CfgDft;

#ifdef __cplusplus
}
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                             MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // _DHCP_CLIENT_TYPES_H_
