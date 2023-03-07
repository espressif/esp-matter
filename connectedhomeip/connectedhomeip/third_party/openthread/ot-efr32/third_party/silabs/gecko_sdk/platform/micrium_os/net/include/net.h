/***************************************************************************//**
 * @file
 * @brief Network Core Header File
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
 * @defgroup NET Network API
 * @brief    Network API
 *
 * @defgroup NET_CORE Network Core API
 * @ingroup  NET
 * @brief    Network Core API
 *
 * @defgroup NET_CORE_INIT Network Initialization API
 * @ingroup  NET_CORE
 * @brief    Network Initialization API
 *
 * @addtogroup NET_CORE_INIT
 * @{
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef  _NET_H_
#define  _NET_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                          INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <net/include/dns_client.h>

#include  <cpu/include/cpu.h>
#include  <common/include/lib_mem.h>
#include  <common/include/rtos_err.h>
#include  <common/include/rtos_types.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                     NET CORE DEFAULT CFG VALUES
 *******************************************************************************************************/

#define  NET_CORE_TASK_CFG_STK_SIZE_ELEMENTS_DFLT                   1024u
#define  NET_CORE_TASK_CFG_STK_PTR_DFLT                             DEF_NULL

#define  NET_CORE_SVC_TASK_CFG_STK_SIZE_ELEMENTS_DFLT               512u
#define  NET_CORE_SVC_TASK_CFG_STK_PTR_DFLT                         DEF_NULL

#define  NET_CORE_MEM_SEG_PTR_DFLT                                  DEF_NULL

/********************************************************************************************************
 ********************************************************************************************************
 *                                             DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                     NET CORE DEFAULT CFG VALUES
 * @brief Net core configuration
 *******************************************************************************************************/
typedef  struct  net_init_cfg {
  DNSc_CFG     DNSc_Cfg;

  CPU_STK_SIZE CoreStkSizeElements;                             ///< Size of the stk, in CPU_STK elements.
  void         *CoreStkPtr;                                     ///< Ptr to base of the stk.
  CPU_STK_SIZE CoreSvcStkSizeElements;                          ///< Size of the stk, in CPU_STK elements.
  void         *CoreSvcStkPtr;                                  ///< Ptr to base of the stk.

  MEM_SEG      *MemSegPtr;                                      ///< Ptr to network core mem seg.
} NET_INIT_CFG;

typedef  struct  net_cfg {
  DNSc_CFG DNSc_Cfg;
} NET_CFG;

#if 0 // TODO_NET coming soon
typedef  struct  net_cfg NET_CFG;

struct  net_cfg {
  CPU_INT08U TmrNbrMax;
  CPU_INT08U IF_NbrMax;
  CPU_INT08U IPv4_AddrNbrMax;
  CPU_INT08U IPv6_AddrNbrMax;
  CPU_INT08U IPv6_RouterNbrMax;
  CPU_INT08U IPv6_PrefixNbrMax;
  CPU_INT08U IPv6_DestCacheNbrMax;
  CPU_INT08U McastHostGrpNbrMax;
};
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                          GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED)
extern const NET_INIT_CFG Net_InitCfgDflt;
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                         FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED)
void Net_ConfigureCoreTaskStk(CPU_STK_SIZE stk_size_elements,
                              void         *p_stk_base);

void Net_ConfigureCoreSvcTaskStk(CPU_STK_SIZE stk_size_elements,
                                 void         *p_stk_base);

void Net_ConfigureMemSeg(MEM_SEG *p_mem_seg);

void Net_ConfigureDNS_Client(DNSc_CFG *p_cfg);
#endif

void Net_Init(RTOS_ERR *p_err);

void Net_CoreTaskPrioSet(RTOS_TASK_PRIO prio,
                         RTOS_ERR       *p_err);

void Net_CoreSvcTaskPrioSet(RTOS_TASK_PRIO prio,
                            RTOS_ERR       *p_err);

#ifdef __cplusplus
}
#endif

/****************************************************************************************************//**
 ********************************************************************************************************
 * @}                                           MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // _NET_H_
