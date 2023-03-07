/*
 * Copyright (c) 2020, Qorvo Inc
 *
 * This software is owned by Qorvo Inc
 * and protected under applicable copyright laws.
 * It is delivered under the terms of the license
 * and is intended and supplied for use solely and
 * exclusively with products manufactured by
 * Qorvo Inc.
 *
 *
 * THIS SOFTWARE IS PROVIDED IN AN "AS IS"
 * CONDITION. NO WARRANTIES, WHETHER EXPRESS,
 * IMPLIED OR STATUTORY, INCLUDING, BUT NOT
 * LIMITED TO, IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 * QORVO INC. SHALL NOT, IN ANY
 * CIRCUMSTANCES, BE LIABLE FOR SPECIAL,
 * INCIDENTAL OR CONSEQUENTIAL DAMAGES,
 * FOR ANY REASON WHATSOEVER.
 *
 */

/** @file "qorvo_internals.h"
 *
 */

#ifndef _QORVO_INTERNALS_H_
#define _QORVO_INTERNALS_H_

/*
 * Enabled components
 */

#define GP_COMP_ASSERT
#define GP_COMP_BASECOMPS
#define GP_COMP_GPHAL_SEC
#define GP_COMP_LOG
#define GP_COMP_LZMA
#define GP_COMP_POOLMEM
#define GP_COMP_SECUREBOOT
#define GP_COMP_SILEXCRYPTOSOC
#define GP_COMP_TLS
#define GP_COMP_UPGRADE
#define GP_COMP_UTILS
#define GP_COMP_WMRK

/*
 * Components numeric ids
 */

#define GP_COMPONENT_ID_APP                                        1
#define GP_COMPONENT_ID_ASSERT                                     29
#define GP_COMPONENT_ID_BASECOMPS                                  35
#define GP_COMPONENT_ID_BSP                                        8
#define GP_COMPONENT_ID_ENCRYPTION                                 124
#define GP_COMPONENT_ID_GPHAL                                      7
#define GP_COMPONENT_ID_HALCORTEXM4                                6
#define GP_COMPONENT_ID_LOG                                        11
#define GP_COMPONENT_ID_LZMA                                       105
#define GP_COMPONENT_ID_PAD                                        126
#define GP_COMPONENT_ID_PD                                         104
#define GP_COMPONENT_ID_POOLMEM                                    106
#define GP_COMPONENT_ID_RANDOM                                     108
#define GP_COMPONENT_ID_RESET                                      33
#define GP_COMPONENT_ID_RXARBITER                                  2
#define GP_COMPONENT_ID_SCHED                                      9
#define GP_COMPONENT_ID_SECUREBOOT                                 5
#define GP_COMPONENT_ID_SILEXCRYPTOSOC                             54
#define GP_COMPONENT_ID_STAT                                       22
#define GP_COMPONENT_ID_TLS                                        14
#define GP_COMPONENT_ID_UPGRADE                                    115
#define GP_COMPONENT_ID_UTILS                                      4
#define GP_COMPONENT_ID_VERSION                                    129
#define GP_COMPONENT_ID_WMRK                                       51

/*
 * Component: gpBsp
 */

#define GP_DIVERSITY_QPG6105DK_B01

/*
 * Component: gpEncryption
 */

#define GP_ENCRYPTION_DIVERSITY_USE_AES_MMO_HW

/*
 * Component: gphal
 */

#define GP_COMP_GPHAL_ES
#define GP_COMP_GPHAL_ES_ABS_EVENT
#define GP_COMP_GPHAL_ES_EXT_EVENT
#define GP_COMP_GPHAL_ES_REL_EVENT
#define GP_DIVERSITY_GPHAL_INTERN
#define GP_DIVERSITY_GPHAL_K8E
#define GP_DIVERSITY_GPHAL_OSCILLATOR_BENCHMARK
#define GP_DIVERSITY_RT_SYSTEM_IN_ROM
#define GP_DIVERSITY_RT_SYSTEM_MACFILTER_IN_ROM
#define GP_DIVERSITY_RT_SYSTEM_PARTS_IN_ROM
#define GP_HAL_DIVERSITY_SEC_CRYPTOSOC

/*
 * Component: gpTls
 */

#define GP_TLS_DIVERSITY_USER_DEFINED_MBEDTLS_CONFIG
#define GP_TLS_DIVERSITY_USE_MBEDTLS_ALT

/*
 * Component: gpUpgrade
 */

#define GP_APP_DIVERSITY_SECURE_BOOTLOADER
#define GP_DIVERSITY_APP_LICENSE_BASED_BOOT
#define GP_DIVERSITY_FLASH_APP_START_OFFSET                        0x6000
#define GP_UPGRADE_DIVERSITY_COMPRESSION

/*
 * Component: gpUtils
 */

#define GP_UTILS_DIVERSITY_LINKED_LIST

/*
 * Component: halCortexM4
 */

#define GP_DIVERSITY_ENABLE_DEFAULT_BOD_HANDLING
#define GP_DIVERSITY_FLASH_BL_SIZE                                 0x2500
#define GP_KX_FLASH_SIZE                                           1024
#define GP_KX_HEAP_SIZE                                            0
#define GP_KX_SYSRAM_SIZE                                          32
#define GP_KX_UCRAM_SIZE                                           96
#define QPG6105

/*
 * Other flags
 */

#define GP_BLE_NR_OF_CONNECTION_COMPLETE_EVENT_BUFFERS             0
#define GP_DIVERSITY_BOOTLOADER_BUILD
#define GP_DIVERSITY_CORTEXM4
#define GP_DIVERSITY_KEEP_NRT_IN_FLASH
#define GP_GIT_SHA                                                 2469438626c13beea2391947d34e05a62039041a
#define GP_GIT_SHA_SHORT                                           2469438
#define GP_HAL_ES_ABS_EVENT_NMBR_OF_EVENTS                         0

#endif //_QORVO_INTERNALS_H_
