/***************************************************************************//**
 * @file
 * @brief OS CPU Port File
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc.  Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement.  This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

/****************************************************************************************************//**
 * @note     (1) This driver targets the following:
 *               Core      : ARMv6M Cortex-M
 *               Toolchain : IAR Compiler
 *******************************************************************************************************/

#ifndef  _ARMV6M_OS_CPU_H
#define  _ARMV6M_OS_CPU_H

/********************************************************************************************************
 *                                             INCLUDE FILES
 *******************************************************************************************************/

#include  <os_cfg.h>
#include  <cpu_cfg.h>

/********************************************************************************************************
 *                                               DEFINES
 *******************************************************************************************************/

#ifdef   OS_CPU_GLOBALS
#define  OS_CPU_EXT
#else
#define  OS_CPU_EXT  extern
#endif

/********************************************************************************************************
 *                                               MACROS
 *******************************************************************************************************/

#define  OS_TASK_SW()               OSCtxSw()

#define  OS_TASK_SW_SYNC()          __ISB()

/********************************************************************************************************
 *                                       TIMESTAMP CONFIGURATION
 *
 * @note     (1) OS_TS_GET() is generally defined as CPU_TS_Get32() to allow CPU timestamp timer to be of
 *               any data type size.
 *
 * @note     (2) For architectures that provide 32-bit or higher precision free running counters
 *               (i.e. cycle count registers):
 *
 *               (a) OS_TS_GET() may be defined as CPU_TS_TmrRd() to improve performance when retrieving
 *                   the timestamp.
 *
 *               (b) CPU_TS_TmrRd() MUST be configured to be greater or equal to 32-bits to avoid
 *                   truncation of TS.
 *******************************************************************************************************/

#if      (OS_CFG_TS_EN == DEF_ENABLED)
#define  OS_TS_GET()               (CPU_TS)CPU_TS_TmrRd()       // See Note #2a.
#else
#define  OS_TS_GET()               (CPU_TS)0u
#endif

#if (CPU_CFG_TS_32_EN == DEF_ENABLED) \
  && (CPU_CFG_TS_TMR_SIZE < CPU_WORD_SIZE_32)
//                                                                 CPU_CFG_TS_TMR_SIZE MUST be >= 32-bit (see Note #2b).
#error  "cpu_cfg.h, CPU_CFG_TS_TMR_SIZE MUST be >= CPU_WORD_SIZE_32"
#endif

/********************************************************************************************************
 *                              OS TICK INTERRUPT PRIORITY CONFIGURATION
 *
 * @note     (1) For systems that don't need any high, real-time priority interrupts; the tick interrupt
 *               should be configured as the highest priority interrupt but won't adversely affect system
 *               operations.
 *
 * @note     (2) For systems that need one or more high, real-time interrupts; these should be configured
 *               higher than the tick interrupt which MAY delay execution of the tick interrupt.
 *
 *               (a) If the higher priority interrupts do NOT continually consume CPU cycles but only
 *                   occasionally delay tick interrupts, then the real-time interrupts can successfully
 *                   handle their intermittent/periodic events with the system not losing tick interrupts
 *                   but only increasing the jitter.
 *
 *               (b) If the higher priority interrupts consume enough CPU cycles to continually delay the
 *                   tick interrupt, then the CPU/system is most likely over-burdened & can't be expected
 *                   to handle all its interrupts/tasks. The system time reference gets compromised as a
 *                   result of losing tick interrupts.
 *******************************************************************************************************/

#ifndef  OS_CPU_CFG_SYSTICK_PRIO
#define  OS_CPU_CFG_SYSTICK_PRIO           0u
#endif

/********************************************************************************************************
 *                                          GLOBAL VARIABLES
 *******************************************************************************************************/

OS_CPU_EXT CPU_STK *OS_CPU_ExceptStkBase;

/********************************************************************************************************
 *                                     EXTERNAL C LANGUAGE LINKAGE
 *
 * @note     (1) C++ compilers MUST 'extern'ally declare ALL C function prototypes & variable/object
 *               declarations for correct C language linkage.
 *******************************************************************************************************/

#ifdef __cplusplus
extern  "C" {                                                   // See Note #1.
#endif

/********************************************************************************************************
 *                                         FUNCTION PROTOTYPES
 *
 *******************************************************************************************************/

//                                                                 See OS_CPU_A.ASM
void OSCtxSw(void);
void OSIntCtxSw(void);
void OSStartHighRdy(void);

void PendSV_Handler(void);
void SysTick_Handler(void);

void OS_CPU_SysTickInit(CPU_INT32U cnts);

/********************************************************************************************************
 *                                   EXTERNAL C LANGUAGE LINKAGE END
 *******************************************************************************************************/

#ifdef __cplusplus
}                                                               // End of 'extern'al C lang linkage.
#endif

/********************************************************************************************************
 *                                             MODULE END
 *******************************************************************************************************/

#endif
