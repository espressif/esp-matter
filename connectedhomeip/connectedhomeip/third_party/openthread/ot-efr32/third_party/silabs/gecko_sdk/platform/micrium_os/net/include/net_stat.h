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

#ifndef  _NET_STAT_H_
#define  _NET_STAT_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                            INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <net/include/net_type.h>

#include  <cpu/include/cpu.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                             DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                              NETWORK STATISTICS POOL QUANTITY DATA TYPE
 *
 * @note    (1) Statistics pool quantity data type MUST be configured with an appropriate-sized network
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
 * @note    (2) NET_STAT_POOL_NBR_MAX  SHOULD be #define'd based on 'NET_STAT_POOL_QTY' data type declared.
 *******************************************************************************************************/

typedef  CPU_INT16U NET_STAT_POOL_QTY;                          ///< Defines max qty of stat pool entries to support.

/********************************************************************************************************
 *                                  NETWORK STATISTICS POOL DATA TYPE
 *
 * @brief Network statistics pool
 *
 * @note    (1) Pool statistic entries MUST ALWAYS be accessed exclusively in critical sections.
 *
 * @note    (2) 'EntriesInit'/'EntriesTot' indicate the initial/current total number of entries in
 *                the statistics pool.
 *             - (a) 'EntriesAvail'/'EntriesUsed' track the current number of entries available/used
 *                    from the statistics pool, while 'EntriesUsedMax' tracks the maximum number of
 *                    entries used at any one time.
 *             - (b) 'EntriesLostCur'/'EntriesLostTot' track the current/total number of unrecoverable
 *                    invalid/corrupted entries lost from the statistics pool.  Lost entries MUST be
 *                    determined by the owner of the statistics pool.
 *             - (c) 'EntriesAllocCtr'/'EntriesDeallocCtr' track the current number of allocated/
 *                    deallocated entries in the statistics pool.
 *
 * @note    (3) Assuming statistics pool are always accessed in critical sections (see Note #2), the
 *               following equations for statistics pools are true at all times :
 *                       EntriesInit = EntriesTot   + EntriesLostTot
 *                       EntriesTot  = EntriesAvail + EntriesUsed
 *                       EntriesUsed = EntriesAllocCtr - EntriesDeallocCtr - EntriesLostCur
 *******************************************************************************************************/

//                                                                 ------------------- NET STAT POOL ------------------
typedef  struct  net_stat_pool {
  NET_STAT_POOL_QTY EntriesInit;                                ///< Init  nbr entries      in   pool.
  NET_STAT_POOL_QTY EntriesTot;                                 ///< Tot   nbr entries      in   pool.
  NET_STAT_POOL_QTY EntriesAvail;                               ///< Avail nbr entries      in   pool.
  NET_STAT_POOL_QTY EntriesUsed;                                ///< Used  nbr entries      from pool.
  NET_STAT_POOL_QTY EntriesUsedMax;                             ///< Max   nbr entries used from pool.
  NET_STAT_POOL_QTY EntriesLostCur;                             ///< Cur   nbr entries lost from pool.
  NET_STAT_POOL_QTY EntriesLostTot;                             ///< Tot   nbr entries lost from pool.

  NET_CTR           EntriesAllocCtr;                            ///< Tot   nbr entries successfully   alloc'd.
  NET_CTR           EntriesDeallocCtr;                          ///< Tot   nbr entries successfully dealloc'd.
} NET_STAT_POOL;

/********************************************************************************************************
 *                                NETWORK STATISTICS COUNTER DATA TYPE
 *
 * @brief Network statistics counter
 *
 * @note    (1) Counter statistic entries MUST ALWAYS be accessed exclusively in critical sections.
 *
 *           (2) (a) 'CurCtr' tracks the current counter value; & ...
 *               (b) 'MaxCtr' tracks the maximum counter value at any one time.
 *******************************************************************************************************/

//                                                                 ------------------- NET STAT CTR -------------------
typedef  struct  net_stat_ctr {
  NET_CTR CurCtr;                                               ///< Cur ctr val.
  NET_CTR MaxCtr;                                               ///< Max ctr val.
} NET_STAT_CTR;

/********************************************************************************************************
 ********************************************************************************************************
 *                                             MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // _NET_STAT_H_
