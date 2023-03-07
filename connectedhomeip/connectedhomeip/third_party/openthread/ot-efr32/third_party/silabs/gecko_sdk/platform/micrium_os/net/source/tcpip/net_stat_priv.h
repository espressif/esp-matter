/***************************************************************************//**
 * @file
 * @brief Network Statistics Management
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

#ifndef  _NET_STAT_PRIV_H_
#define  _NET_STAT_PRIV_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  "../../include/net_stat.h"

#include  "../../include/net_cfg_net.h"
#include  "net_ctr_priv.h"
#include  "net_type_priv.h"

#include  <cpu/include/cpu.h>
#include  <common/include/rtos_err.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                               NETWORK STATISTICS POOL QUANTITY DATA TYPE
 *
 * Note(s) : (1) Statistics pool quantity data type MUST be configured with an appropriate-sized network
 *               data type large enough to perform calculations on the following data types :
 *
 *               (a) NET_BUF_QTY
 *               (b) NET_TMR_QTY
 *               (c) NET_CONN_QTY
 *               (d) NET_CONN_LIST_QTY
 *               (e) NET_ARP_CACHE_QTY
 *               (f) NET_ICMP_SRC_QUENCH_QTY
 *               (g) NET_TCP_CONN_QTY
 *               (h) NET_SOCK_QTY
 *
 *           (2) NET_STAT_POOL_NBR_MAX  SHOULD be #define'd based on 'NET_STAT_POOL_QTY' data type declared.
 *******************************************************************************************************/

#define  NET_STAT_POOL_NBR_MIN                             1
#define  NET_STAT_POOL_NBR_MAX           DEF_INT_16U_MAX_VAL    // See Note #2.

#define  NET_STAT_POOL_MIN_VAL                             0
#define  NET_STAT_POOL_MAX_VAL    (NET_STAT_POOL_NBR_MAX - 1)

/********************************************************************************************************
 ********************************************************************************************************
 *                                           FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                           INTERNAL FUNCTIONS
 *******************************************************************************************************/

void NetStat_Init(void);

//                                                                 --------- NET STAT CTR  FNCTS ----------
void NetStat_CtrInit(NET_STAT_CTR *p_stat_ctr);

void NetStat_CtrClr(NET_STAT_CTR *p_stat_ctr);

void NetStat_CtrReset(NET_STAT_CTR *p_stat_ctr);

void NetStat_CtrResetMax(NET_STAT_CTR *p_stat_ctr);

void NetStat_CtrInc(NET_STAT_CTR *p_stat_ctr);

void NetStat_CtrDec(NET_STAT_CTR *p_stat_ctr);

//                                                                 --------- NET STAT POOL FNCTS ----------
void NetStat_PoolInit(NET_STAT_POOL     *p_stat_pool,
                      NET_STAT_POOL_QTY nbr_avail);

void NetStat_PoolClr(NET_STAT_POOL *p_stat_pool);

void NetStat_PoolReset(NET_STAT_POOL *p_stat_pool);

void NetStat_PoolResetUsedMax(NET_STAT_POOL *p_stat_pool);

void NetStat_PoolEntryUsedInc(NET_STAT_POOL *p_stat_pool,
                              RTOS_ERR      *p_err);

void NetStat_PoolEntryUsedDec(NET_STAT_POOL *p_stat_pool,
                              RTOS_ERR      *p_err);

void NetStat_PoolEntryLostInc(NET_STAT_POOL *p_stat_pool,
                              RTOS_ERR      *p_err);

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // _NET_STAT_PRIV_H_
