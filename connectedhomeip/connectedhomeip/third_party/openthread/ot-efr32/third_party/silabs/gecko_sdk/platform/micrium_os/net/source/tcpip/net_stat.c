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
 *                                       DEPENDENCIES & AVAIL CHECK(S)
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <rtos_description.h>

#if (defined(RTOS_MODULE_NET_AVAIL))

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <em_core.h>

#include  "net_stat_priv.h"
#include  "net_ctr_priv.h"

#include  <common/source/rtos/rtos_utils_priv.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  LOG_DFLT_CH                                  (NET)
#define  RTOS_MODULE_CUR                               RTOS_CFG_MODULE_NET

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                               NetStat_Init()
 *
 * @brief    (1) Initialize Network Statistic Management Module :
 *               Module initialization NOT yet required/implemented
 *******************************************************************************************************/
void NetStat_Init(void)
{
}

/****************************************************************************************************//**
 *                                               NetStat_CtrInit()
 *
 * @brief    Initialize a statistics counter.
 *
 * @param    p_stat_ctr  Pointer to a statistics counter (see Note #1).
 *
 * @note     (1) Assumes 'p_stat_ctr' points to valid statistics counter (if non-NULL).
 *
 * @note     (2) Statistic counters MUST ALWAYS be accessed exclusively in critical sections.
 *******************************************************************************************************/
void NetStat_CtrInit(NET_STAT_CTR *p_stat_ctr)
{
  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_ATOMIC();
  p_stat_ctr->CurCtr = 0u;
  p_stat_ctr->MaxCtr = 0u;
  CORE_EXIT_ATOMIC();
}

/****************************************************************************************************//**
 *                                               NetStat_CtrClr()
 *
 * @brief    Clear a statistics counter.
 *
 * @param    p_stat_ctr  Pointer to a statistics counter (see Note #1).
 *
 * @note     (1) Assumes 'p_stat_ctr' points to valid statistics counter (if non-NULL).
 *
 * @note     (2) Statistic counters MUST ALWAYS be accessed exclusively in critical sections.
 *******************************************************************************************************/
void NetStat_CtrClr(NET_STAT_CTR *p_stat_ctr)
{
  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_ATOMIC();
  p_stat_ctr->CurCtr = 0u;
  p_stat_ctr->MaxCtr = 0u;
  CORE_EXIT_ATOMIC();
}

/****************************************************************************************************//**
 *                                           NetStat_CtrReset()
 *
 * @brief    Reset a statistics counter.
 *
 * @param    p_stat_ctr  Pointer to a statistics counter.
 *
 * @note     (1) Statistic counters MUST ALWAYS be accessed exclusively in critical sections.
 *******************************************************************************************************/
void NetStat_CtrReset(NET_STAT_CTR *p_stat_ctr)
{
  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_ATOMIC();
  p_stat_ctr->CurCtr = 0u;
  p_stat_ctr->MaxCtr = 0u;
  CORE_EXIT_ATOMIC();
}

/****************************************************************************************************//**
 *                                           NetStat_CtrResetMax()
 *
 * @brief    Reset a statistics counter's maximum number of counts.
 *               - (1) Resets maximum number of counts to the current number of counts.
 *
 * @param    p_stat_ctr  Pointer to a statistics counter.
 *
 * @note     (2) Statistic counters MUST ALWAYS be accessed exclusively in critical sections.
 *******************************************************************************************************/
void NetStat_CtrResetMax(NET_STAT_CTR *p_stat_ctr)
{
  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_ATOMIC();
  p_stat_ctr->MaxCtr = p_stat_ctr->CurCtr;                      // Reset max cnts (see Note #1).
  CORE_EXIT_ATOMIC();
}

/****************************************************************************************************//**
 *                                               NetStat_CtrInc()
 *
 * @brief    Increment a statistics counter.
 *
 * @param    p_stat_ctr  Pointer to a statistics counter.
 *
 * @note     (1) Statistic counters MUST ALWAYS be accessed exclusively in critical sections.
 *
 * @note     (2) Statistic counter increment overflow prevented but ignored.
 *
 *                       See also 'NetStat_CtrDec()  Note #2'.
 *******************************************************************************************************/
void NetStat_CtrInc(NET_STAT_CTR *p_stat_ctr)
{
  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_ATOMIC();
  if (p_stat_ctr->CurCtr < NET_CTR_MAX) {                       // See Note #2.
    p_stat_ctr->CurCtr++;

    if (p_stat_ctr->MaxCtr < p_stat_ctr->CurCtr) {              // If max cnt < cur cnt, set new max cnt.
      p_stat_ctr->MaxCtr = p_stat_ctr->CurCtr;
    }
  }
  CORE_EXIT_ATOMIC();
}

/****************************************************************************************************//**
 *                                               NetStat_CtrDec()
 *
 * @brief    Decrement a statistics counter.
 *
 * @param    p_stat_ctr  Pointer to a statistics counter.
 *
 * @note     (1) Statistic counters MUST ALWAYS be accessed exclusively in critical sections.
 *
 * @note     (2) Statistic counter decrement underflow prevented but ignored.
 *
 *                       See also 'NetStat_CtrInc()  Note #2'.
 *******************************************************************************************************/
void NetStat_CtrDec(NET_STAT_CTR *p_stat_ctr)
{
  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_ATOMIC();
  if (p_stat_ctr->CurCtr > NET_CTR_MIN) {                       // See Note #2.
    p_stat_ctr->CurCtr--;
  }
  CORE_EXIT_ATOMIC();
}

/****************************************************************************************************//**
 *                                           NetStat_PoolInit()
 *
 * @brief    Initialize a statistics pool.
 *
 * @param    p_stat_pool     Pointer to a statistics pool (see Note #1).
 *
 * @param    nbr_avail       Total number of available statistics pool entries.
 *
 * @note     (1) Assumes 'p_stat_pool' points to valid statistics pool (if non-NULL).
 *
 * @note     (2) Pool statistic entries MUST ALWAYS be accessed exclusively in critical sections.
 *******************************************************************************************************/
void NetStat_PoolInit(NET_STAT_POOL     *p_stat_pool,
                      NET_STAT_POOL_QTY nbr_avail)
{
  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_ATOMIC();
  p_stat_pool->EntriesInit = nbr_avail;                         // Init nbr of pool  entries is also ...
  p_stat_pool->EntriesTot = nbr_avail;                          // Tot  nbr of pool  entries is also ...
  p_stat_pool->EntriesAvail = nbr_avail;                        // Init nbr of avail entries.
  p_stat_pool->EntriesUsed = 0u;
  p_stat_pool->EntriesUsedMax = 0u;
  p_stat_pool->EntriesLostCur = 0u;
  p_stat_pool->EntriesLostTot = 0u;
  p_stat_pool->EntriesAllocCtr = 0u;
  p_stat_pool->EntriesDeallocCtr = 0u;
  CORE_EXIT_ATOMIC();
}

/****************************************************************************************************//**
 *                                               NetStat_PoolClr()
 *
 * @brief    Clear a statistics pool.
 *
 * @param    p_stat_pool     Pointer to a statistics pool (see Note #1).
 *
 * @note     (1) Assumes 'p_stat_pool' points to valid statistics pool (if non-NULL).
 *
 * @note     (2) Pool statistic entries MUST ALWAYS be accessed exclusively in critical sections.
 *******************************************************************************************************/
void NetStat_PoolClr(NET_STAT_POOL *p_stat_pool)
{
  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_ATOMIC();
  p_stat_pool->EntriesInit = 0u;
  p_stat_pool->EntriesTot = 0u;
  p_stat_pool->EntriesAvail = 0u;
  p_stat_pool->EntriesUsed = 0u;
  p_stat_pool->EntriesUsedMax = 0u;
  p_stat_pool->EntriesLostCur = 0u;
  p_stat_pool->EntriesLostTot = 0u;
  p_stat_pool->EntriesAllocCtr = 0u;
  p_stat_pool->EntriesDeallocCtr = 0u;
  CORE_EXIT_ATOMIC();
}

/****************************************************************************************************//**
 *                                           NetStat_PoolReset()
 *
 * @brief    Reset a statistics pool.
 *               - (1) Assumes object pool is also reset; otherwise, statistics pool will NOT accurately
 *                     reflect the state of the object pool.
 *
 * @param    p_stat_pool     Pointer to a statistics pool.
 *
 * @note     (2) Pool statistic entries MUST ALWAYS be accessed exclusively in critical sections.
 *******************************************************************************************************/
void NetStat_PoolReset(NET_STAT_POOL *p_stat_pool)
{
  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_ATOMIC();
  p_stat_pool->EntriesAvail = p_stat_pool->EntriesTot;
  p_stat_pool->EntriesUsed = 0u;
  p_stat_pool->EntriesUsedMax = 0u;
  p_stat_pool->EntriesLostCur = 0u;
  p_stat_pool->EntriesAllocCtr = 0u;
  p_stat_pool->EntriesDeallocCtr = 0u;
  CORE_EXIT_ATOMIC();
}

/****************************************************************************************************//**
 *                                       NetStat_PoolResetUsedMax()
 *
 * @brief    Reset a statistics pool's maximum number of entries used.
 *               - (1) Resets maximum number of entries used to the current number of entries used.
 *
 * @param    p_stat_pool     Pointer to a statistics pool.
 *
 * @note     (2) Pool statistic entries MUST ALWAYS be accessed exclusively in critical sections.
 *******************************************************************************************************/
void NetStat_PoolResetUsedMax(NET_STAT_POOL *p_stat_pool)
{
  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_ATOMIC();
  p_stat_pool->EntriesUsedMax = p_stat_pool->EntriesUsed;       // Reset nbr max used (see Note #1).
  CORE_EXIT_ATOMIC();
}

/****************************************************************************************************//**
 *                                       NetStat_PoolEntryUsedInc()
 *
 * @brief    Increment a statistics pool's number of 'Used' entries.
 *
 * @param    p_stat_pool     Pointer to a statistics pool.
 *
 * @param    p_err           Pointer to variable that will receive the return error code from this function.
 *
 * @note     (1) Pool statistic entries MUST ALWAYS be accessed exclusively in critical sections.
 *******************************************************************************************************/
void NetStat_PoolEntryUsedInc(NET_STAT_POOL *p_stat_pool,
                              RTOS_ERR      *p_err)
{
  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_ATOMIC();
  if (p_stat_pool->EntriesAvail <= 0) {
    CORE_EXIT_ATOMIC();
    RTOS_ERR_SET(*p_err, RTOS_ERR_POOL_EMPTY);
    goto exit;
  }

  //                                                               If any stat pool entry avail,             ...
  p_stat_pool->EntriesAvail--;                                  // ... adj nbr of avail/used entries in pool ...
  p_stat_pool->EntriesUsed++;
  p_stat_pool->EntriesAllocCtr++;                               // ... & inc tot nbr of alloc'd entries.
  if (p_stat_pool->EntriesUsedMax < p_stat_pool->EntriesUsed) {   // If max used < nbr used, set new max used.
    p_stat_pool->EntriesUsedMax = p_stat_pool->EntriesUsed;
  }
  CORE_EXIT_ATOMIC();

exit:
  return;
}

/****************************************************************************************************//**
 *                                       NetStat_PoolEntryUsedDec()
 *
 * @brief    Decrement a statistics pool's number of 'Used' entries.
 *
 * @param    p_stat_pool     Pointer to a statistics pool.
 *
 * @param    p_err           Pointer to variable that will receive the return error code from this function.
 *
 * @note     (1) Pool statistic entries MUST ALWAYS be accessed exclusively in critical sections.
 *******************************************************************************************************/
void NetStat_PoolEntryUsedDec(NET_STAT_POOL *p_stat_pool,
                              RTOS_ERR      *p_err)
{
  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_ATOMIC();
  if (p_stat_pool->EntriesUsed <= 0) {
    CORE_EXIT_ATOMIC();
    RTOS_ERR_SET(*p_err, RTOS_ERR_POOL_FULL);
    goto exit;
  }
  //                                                               If any stat pool entry used,              ...
  p_stat_pool->EntriesAvail++;                                  // ... adj nbr of avail/used entries in pool ...
  p_stat_pool->EntriesUsed--;
  p_stat_pool->EntriesDeallocCtr++;                             // ... & inc tot nbr of dealloc'd entries.

  CORE_EXIT_ATOMIC();

exit:
  return;
}

/****************************************************************************************************//**
 *                                       NetStat_PoolEntryLostInc()
 *
 * @brief    Increment a statistics pool's number of 'Lost' entries.
 *
 * @param    p_stat_pool     Pointer to a statistics pool.
 *
 * @param    p_err           Pointer to variable that will receive the return error code from this function.
 *
 * @note     (1) Pool statistic entries MUST ALWAYS be accessed exclusively in critical sections.
 *******************************************************************************************************/
void NetStat_PoolEntryLostInc(NET_STAT_POOL *p_stat_pool,
                              RTOS_ERR      *p_err)
{
  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_ATOMIC();
  if (p_stat_pool->EntriesTot <= 0) {
    CORE_EXIT_ATOMIC();
    RTOS_ERR_SET(*p_err, RTOS_ERR_POOL_EMPTY);
    goto exit;
  }

  if (p_stat_pool->EntriesUsed <= 0) {
    CORE_EXIT_ATOMIC();
    RTOS_ERR_SET(*p_err, RTOS_ERR_POOL_FULL);
    goto exit;
  }
  //                                                               If   tot stat pool entries > 0  ...
  //                                                               ... & any stat pool entry used, ...
  p_stat_pool->EntriesUsed--;                                   // ... adj nbr used/total/lost entries in pool.
  p_stat_pool->EntriesTot--;
  p_stat_pool->EntriesLostCur++;
  p_stat_pool->EntriesLostTot++;

  CORE_EXIT_ATOMIC();

exit:
  return;
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // RTOS_MODULE_NET_AVAIL
