/***************************************************************************//**
 * @file
 * @brief Network TCP Layer - (Transmission Control Protocol)
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

/****************************************************************************************************//**
 * @defgroup NET_CORE_TCP TCP API
 * @ingroup  NET_CORE
 * @brief    TCP API
 *
 * @addtogroup NET_CORE_TCP
 * @{
 ********************************************************************************************************
 * @note     (1) Supports Transmission Control Protocol as described in RFC #793 with the following
 *               restrictions/constraints :
 *             - (a) TCP Security & Precedence NOT supported               RFC # 793, Section 3.6
 *             - (b) TCP Urgent Data           NOT supported               RFC # 793, Section 3.7
 *                                                                          'The Communication of
 *                                                                            Urgent Information'
 *             - (c) The following TCP options NOT supported :
 *                 - (1) Window Scale                                      RFC #1072, Section 2
 *                                                                         RFC #1323, Section 2
 *                 - (2) Selective Acknowledgement (SACK)                  RFC #1072, Section 3
 *                                                                         RFC #2018
 *                                                                         RFC #2883
 *                 - (3) TCP Echo                                          RFC #1072, Section 4
 *                 - (4) Timestamp                                         RFC #1323, Section 3.2
 *                 - (5) Protection Against Wrapped Sequences (PAWS)       RFC #1323, Section 4
 *             - (d) IP-Options-to-TCP-Connection                          RFC #1122, Section 4.2.3.8
 *                          Handling NOT supported #### NET-804
 *             - (e) ICMP-Error-Message-to-TCP-Connection                  RFC #1122, Section 4.2.3.9
 *                          Handling NOT currently supported #### NET-805
 * @note     (2) TCP Layer assumes/requires Network Socket Layer.
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef  _NET_TCP_H_
#define  _NET_TCP_H_

#include  "net_cfg_net.h"

#ifdef   NET_TCP_MODULE_EN

/********************************************************************************************************
 ********************************************************************************************************
 *                                            INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <net/include/net_stat.h>
#include  <net/include/net_type.h>

#include  <cpu/include/cpu.h>
#include  <common/include/rtos_err.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                             DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                               TCP CONNECTION IDENTIFICATION DATA TYPE
 *******************************************************************************************************/

typedef  CPU_INT16S NET_TCP_CONN_ID;

/********************************************************************************************************
 *                                       TCP TIMEOUT DATA TYPES
 *******************************************************************************************************/

typedef  CPU_INT32U NET_TCP_TIMEOUT_MS;
typedef  CPU_INT16U NET_TCP_TIMEOUT_SEC;

/********************************************************************************************************
 *                                   TCP CONNECTION STATE DATA TYPE
 *
 * Note(s) : (1) See the following RFC's for TCP state machine summary :
 *
 *               (a) RFC # 793; Sections 3.2, 3.4, 3.5, 3.9
 *               (b) RFC #1122; Sections 4.2.2.8, 4.2.2.10, 4.2.2.11, 4.2.2.13, 4.2.2.18, 4.2.2.20
 *
 *           (2) (a) Additional closing-data-available state used for closing connections to allow the
 *                   application layer to receive any remaining data.
 *
 *                   See also 'net_tcp.c  NetTCP_RxPktConnHandlerFinWait1()  Note #2f5A2',
 *                            'net_tcp.c  NetTCP_RxPktConnHandlerFinWait2()  Note #2f5B',
 *                            'net_tcp.c  NetTCP_RxPktConnHandlerClosing()   Note #2d2B2a1B',
 *                          & 'net_tcp.c  NetTCP_RxPktConnHandlerLastAck()   Note #2d2A1b'.
 *******************************************************************************************************/

typedef  enum  net_tcp_state {
  NET_TCP_CONN_STATE_NONE = 1u,
  NET_TCP_CONN_STATE_FREE = 2u,

  NET_TCP_CONN_STATE_CLOSED = 10u,

  NET_TCP_CONN_STATE_LISTEN = 20u,

  NET_TCP_CONN_STATE_SYNC_RXD = 30u,
  NET_TCP_CONN_STATE_SYNC_RXD_PASSIVE = 31u,
  NET_TCP_CONN_STATE_SYNC_RXD_ACTIVE = 32u,

  NET_TCP_CONN_STATE_SYNC_TXD = 35u,

  NET_TCP_CONN_STATE_CONN = 40u,

  NET_TCP_CONN_STATE_FIN_WAIT_1 = 50u,
  NET_TCP_CONN_STATE_FIN_WAIT_2 = 51u,
  NET_TCP_CONN_STATE_CLOSING = 52u,
  NET_TCP_CONN_STATE_TIME_WAIT = 53u,

  NET_TCP_CONN_STATE_CLOSE_WAIT = 55u,
  NET_TCP_CONN_STATE_LAST_ACK = 56u,

  NET_TCP_CONN_STATE_CLOSING_DATA_AVAIL = 59u
} NET_TCP_CONN_STATE;

/********************************************************************************************************
 ********************************************************************************************************
 *                                         FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

CPU_BOOLEAN NetTCP_ConnCfgIdleTimeout(NET_TCP_CONN_ID     conn_id_tcp,
                                      NET_TCP_TIMEOUT_SEC timeout_sec,
                                      RTOS_ERR            *p_err);

CPU_BOOLEAN NetTCP_ConnCfgMaxSegSizeLocal(NET_TCP_CONN_ID  conn_id_tcp,
                                          NET_TCP_SEG_SIZE max_seg_size,
                                          RTOS_ERR         *p_err);

CPU_BOOLEAN NetTCP_ConnCfgRxWinSize(NET_TCP_CONN_ID  conn_id_tcp,
                                    NET_TCP_WIN_SIZE win_size,
                                    RTOS_ERR         *p_err);

CPU_BOOLEAN NetTCP_ConnCfgTxWinSize(NET_TCP_CONN_ID  conn_id_tcp,
                                    NET_TCP_WIN_SIZE win_size,
                                    RTOS_ERR         *p_err);

CPU_BOOLEAN NetTCP_ConnCfgTxNagleEn(NET_TCP_CONN_ID conn_id_tcp,
                                    CPU_BOOLEAN     nagle_en,
                                    RTOS_ERR        *p_err);

CPU_BOOLEAN NetTCP_ConnCfgTxAckImmedRxdPushEn(NET_TCP_CONN_ID conn_id_tcp,
                                              CPU_BOOLEAN     tx_immed_ack_en,
                                              RTOS_ERR        *p_err);

CPU_BOOLEAN NetTCP_ConnCfgTxAckDlyTimeout(NET_TCP_CONN_ID    conn_id_tcp,
                                          NET_TCP_TIMEOUT_MS timeout_ms,
                                          RTOS_ERR           *p_err);

CPU_BOOLEAN NetTCP_ConnCfgTxKeepAliveEn(NET_TCP_CONN_ID conn_id_tcp,
                                        CPU_BOOLEAN     keep_alive_en,
                                        RTOS_ERR        *p_err);

CPU_BOOLEAN NetTCP_ConnCfgTxKeepAliveRetryTimeout(NET_TCP_CONN_ID     conn_id_tcp,
                                                  NET_TCP_TIMEOUT_SEC timeout_sec,
                                                  RTOS_ERR            *p_err);

CPU_BOOLEAN NetTCP_ConnCfgTxKeepAliveTh(NET_TCP_CONN_ID conn_id_tcp,
                                        NET_PKT_CTR     nbr_max_keep_alive,
                                        RTOS_ERR        *p_err);

CPU_BOOLEAN NetTCP_ConnCfgReTxMaxTh(NET_TCP_CONN_ID conn_id_tcp,
                                    NET_PKT_CTR     nbr_max_re_tx,
                                    RTOS_ERR        *p_err);

CPU_BOOLEAN NetTCP_ConnCfgReTxMaxTimeout(NET_TCP_CONN_ID     conn_id_tcp,
                                         NET_TCP_TIMEOUT_SEC timeout_sec,
                                         RTOS_ERR            *p_err);

CPU_BOOLEAN NetTCP_ConnCfgMSL_Timeout(NET_TCP_CONN_ID     conn_id_tcp,
                                      NET_TCP_TIMEOUT_SEC msl_timeout_sec,
                                      RTOS_ERR            *p_err);

NET_STAT_POOL NetTCP_ConnPoolStatGet(void);

void NetTCP_ConnPoolStatResetMaxUsed(void);

NET_TCP_CONN_STATE NetTCP_ConnStateGet(NET_TCP_CONN_ID conn_id);

#ifdef __cplusplus
}
#endif

/****************************************************************************************************//**
 ********************************************************************************************************
 * @}                                          MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // NET_TCP_MODULE_EN
#endif // _NET_TCP_H_
