/***************************************************************************//**
 * @file
 * @brief Network Counter Management
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

#include  <net/include/net_cfg_net.h>
#include  "net_ctr_priv.h"
#include  "net_priv.h"

#include  <cpu/include/cpu.h>
#include  <common/include/lib_mem.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  LOG_DFLT_CH                                  (NET)
#define  RTOS_MODULE_CUR                               RTOS_CFG_MODULE_NET

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                           STATISTIC COUNTERS
 *******************************************************************************************************/

#if (NET_CTR_CFG_STAT_EN == DEF_ENABLED)
NET_CTR_STATS Net_StatCtrs;
#endif

/********************************************************************************************************
 *                                           ERROR COUNTERS
 *******************************************************************************************************/

#if (NET_CTR_CFG_ERR_EN == DEF_ENABLED)
NET_CTR_ERRS Net_ErrCtrs;
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                               NetCtr_Init()
 *
 * @brief    (1) Initialize Network Counter Management Module :
 *               - (a) Initialize network statistics counters
 *               - (b) Initialize network error      counters
 *******************************************************************************************************/
void NetCtr_Init(MEM_SEG  *p_mem_seg,
                 RTOS_ERR *p_err)
{
  PP_UNUSED_PARAM(p_mem_seg);
  PP_UNUSED_PARAM(p_err);

#if (NET_CTR_CFG_STAT_EN == DEF_ENABLED)                        // ---------------- INIT NET STAT CTRS ----------------
  Mem_Clr(&Net_StatCtrs,
          sizeof(Net_StatCtrs));
#endif

#if (NET_CTR_CFG_ERR_EN == DEF_ENABLED)                         // ---------------- INIT NET ERR  CTRS ----------------
  Mem_Clr(&Net_ErrCtrs,
          sizeof(Net_ErrCtrs));
#endif

#if (NET_CTR_CFG_STAT_EN == DEF_ENABLED)
  Net_StatCtrs.IFs.IF = (NET_CTR_IF_STATS *)Mem_SegAlloc("Stat Counter table for Interfaces",
                                                         p_mem_seg,
                                                         sizeof(NET_CTR_IF_STATS) * Net_CoreDataPtr->IF_NbrTot,
                                                         p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }
#endif

#if (NET_CTR_CFG_ERR_EN == DEF_ENABLED)
  Net_ErrCtrs.IFs.IF = (NET_CTR_IF_ERRS *)Mem_SegAlloc("Error Counter table for Interfaces",
                                                       p_mem_seg,
                                                       sizeof(NET_CTR_IF_ERRS) * Net_CoreDataPtr->IF_NbrTot,
                                                       p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }
#endif
}

/****************************************************************************************************//**
 *                                               NetCtr_Inc()
 *
 * @brief    Increment a network counter.
 *
 * @param    pctr    Pointer to a network counter.
 *
 * @note     (1) Network counter variables MUST ALWAYS be accessed exclusively in critical sections.
 *               See also 'net_ctr.h  NETWORK COUNTER MACRO'S  Note #1a'.
 *******************************************************************************************************/

#ifdef  NET_CTR_MODULE_EN
void NetCtr_Inc(NET_CTR *p_ctr)
{
  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_ATOMIC();
  (*p_ctr)++;
  CORE_EXIT_ATOMIC();
}
#endif

/****************************************************************************************************//**
 *                                               NetCtr_IncLarge()
 *
 * @brief    Increment a large network counter.
 *
 * @param    pctr_hi     Pointer to high half of a large network counter.
 *
 * @param    pctr_lo     Pointer to low  half of a large network counter.
 *
 * @note     (1) Network counter variables MUST ALWAYS be accessed exclusively in critical sections.
 *               See also 'net_ctr.h  NETWORK COUNTER MACRO'S  Note #1b'.
 *******************************************************************************************************/

#ifdef  NET_CTR_MODULE_EN
void NetCtr_IncLarge(NET_CTR *p_ctr_hi,
                     NET_CTR *p_ctr_lo)
{
  (*p_ctr_lo)++;                                                // Inc lo-half ctr.
  if (*p_ctr_lo == 0u) {                                        // If  lo-half ctr ovfs, ...
    (*p_ctr_hi)++;                                              // inc hi-half ctr.
  }
}
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // RTOS_MODULE_NET_AVAIL
