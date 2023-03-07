/***************************************************************************//**
 * @file
 * @brief CPU Configuration - Configuration Template File
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

// <<< Use Configuration Wizard in Context Menu >>>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef  _CPU_CFG_H_
#define  _CPU_CFG_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <cpu/include/cpu_def.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                        CPU NAME CONFIGURATION
 ********************************************************************************************************
 *******************************************************************************************************/

// <q CPU_CFG_NAME_EN> Provide name for CPU
// <i> Enable CPU host name feature for CPU host name storage and CPU host name API functions.
// <i> Default: 0
#define  CPU_CFG_NAME_EN                                    0

// <o CPU_CFG_NAME_SIZE> Size of CPU name
// <i> Desired ASCII string size of the CPU host name, including the terminating NULL character.
// <i> Default: 16
#define  CPU_CFG_NAME_SIZE                                  16

/********************************************************************************************************
 ********************************************************************************************************
 *                                     CPU TIMESTAMP CONFIGURATION
 ********************************************************************************************************
 *******************************************************************************************************/

// <q CPU_CFG_TS_32_EN> Timestamp on 32 bits
// <i> Enable 32-bits CPU timestamp feature.
// <i> Default: 0
#define  CPU_CFG_TS_32_EN                                   0

// <q CPU_CFG_TS_64_EN> Timestamp on 64 bits
// <i> Enable 64-bits CPU timestamp feature.
// <i> Default: 0
#define  CPU_CFG_TS_64_EN                                   0

// <o CPU_CFG_TS_TMR_SIZE> CPU timestamp timer's word size
//   <CPU_WORD_SIZE_08=> 8 bits
//   <CPU_WORD_SIZE_16=> 16 bits
//   <CPU_WORD_SIZE_32=> 32 bits
//   <CPU_WORD_SIZE_64=> 64 bits
// <i> If the size of the CPU timestamp timer is not a binary multiple of 8-bit octets (e.g. 20-bits or even 24-bits),
// <i> then the next lower, binary-multiple octet word size SHOULD be configured (e.g. to 16-bits).
// <i> However, the minimum supported word size for CPU timestamp timers is 8-bits.
// <i> Default: CPU_WORD_SIZE_32
#define  CPU_CFG_TS_TMR_SIZE                                CPU_WORD_SIZE_32

/********************************************************************************************************
 ********************************************************************************************************
 *                                           CACHE MANAGEMENT
 *
 * Note(s) : (1) Defining CPU_CFG_CACHE_MGMT_EN to DEF_ENABLED only enables the cache management
 *               function. Caches are assumed to be configured and enabled by the time CPU_init() is
 *               called.
 ********************************************************************************************************
 *******************************************************************************************************/

// <q CPU_CFG_TS_64_EN> Cache Management
// <i> Enable the cache management function.
// <i> Caches are assumed to be configured and enabled by the time CPU_init() is called.
// <i> Default: 0
#define  CPU_CFG_CACHE_MGMT_EN                              0

/********************************************************************************************************
 ********************************************************************************************************
 *                                             MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // End of cpu_cfg.h module include.

// <<< end of configuration section >>>
