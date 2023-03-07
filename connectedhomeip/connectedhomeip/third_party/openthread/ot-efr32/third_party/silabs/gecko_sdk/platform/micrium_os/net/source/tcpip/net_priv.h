/***************************************************************************//**
 * @file
 * @brief Network Header File
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

#ifndef  _NET_PRIV_H_
#define  _NET_PRIV_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                           NETWORK INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  "../../include/net_cfg_net.h"
#include  "net_type_priv.h"

#include  <net/source/util/net_svc_task_priv.h>

#include  <cpu/include/cpu.h>
#include  <common/include/lib_mem.h>
#include  <common/include/rtos_err.h>
#include  <common/include/rtos_path.h>
#include  <net_cfg.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

typedef  struct  net_core_data {
  MEM_SEG             *CoreMemSegPtr;

  KAL_LOCK_HANDLE     GlobalLock;
  void                *GlobaLockFcntPtr;

  CPU_INT08U          IF_NbrTot;
  CPU_INT08U          IF_NbrCfgd;
  CPU_INT08U          IF_TxSuspendTimeout_ms;

  NET_SVC_TASK_HANDLE SvcTaskHandle;
} NET_CORE_DATA;

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

extern NET_CORE_DATA *Net_CoreDataPtr;

/********************************************************************************************************
 ********************************************************************************************************
 *                                           FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

void NetCore_InitModules(MEM_SEG  *p_mem_seg,
                         RTOS_ERR *p_err);

void Net_InitCompWait(void);                                    // Wait  until network initialization is complete.

void Net_InitCompSignal(void);                                  // Signal that network initialization is complete.

void Net_GlobalLockAcquire(void *p_fcnt);                       // Acquire access to network protocol suite.

void Net_GlobalLockRelease(void);                               // Release access to network protocol suite.

void Net_TimeDly(CPU_INT32U time_dly_sec,
                 CPU_INT32U time_dly_us,
                 RTOS_ERR   *p_err);

/********************************************************************************************************
 ********************************************************************************************************
 *                                       NETWORK CONFIGURATION ERRORS
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef  NET_CFG_OPTIMIZE_ASM_EN
#error  "NET_CFG_OPTIMIZE_ASM_EN        not #define'd in 'net_cfg.h'"
#error  "                         [MUST be  DEF_DISABLED]           "
#error  "                         [     ||  DEF_ENABLED ]           "

#elif  ((NET_CFG_OPTIMIZE_ASM_EN != DEF_DISABLED) \
  && (NET_CFG_OPTIMIZE_ASM_EN != DEF_ENABLED))
#error  "NET_CFG_OPTIMIZE_ASM_EN  illegally #define'd in 'net_cfg.h'"
#error  "                         [MUST be  DEF_DISABLED]           "
#error  "                         [     ||  DEF_ENABLED ]           "
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // _NET_PRIV_H_
