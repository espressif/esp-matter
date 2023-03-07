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
#define GP_COMP_BLE
#define GP_COMP_BLEACTIVITYMANAGER
#define GP_COMP_BLEADDRESSRESOLVER
#define GP_COMP_BLEADVERTISER
#define GP_COMP_BLECOMPS
#define GP_COMP_BLECONFIG
#define GP_COMP_BLECONNECTIONMANAGER
#define GP_COMP_BLEDATACHANNELRXQUEUE
#define GP_COMP_BLEDATACHANNELTXQUEUE
#define GP_COMP_BLEDATACOMMON
#define GP_COMP_BLEDATARX
#define GP_COMP_BLEDATATX
#define GP_COMP_BLELLCP
#define GP_COMP_BLELLCPFRAMEWORK
#define GP_COMP_BLELLCPPROCEDURES
#define GP_COMP_BLESECURITYCOPROCESSOR
#define GP_COMP_COM
#define GP_COMP_ECC
#define GP_COMP_ENCRYPTION
#define GP_COMP_EXTSTORAGE
#define GP_COMP_GPHAL
#define GP_COMP_GPHAL_BLE
#define GP_COMP_GPHAL_MAC
#define GP_COMP_GPHAL_PBM
#define GP_COMP_GPHAL_RADIO
#define GP_COMP_GPHAL_SEC
#define GP_COMP_HALCORTEXM4
#define GP_COMP_HCI
#define GP_COMP_JUMPTABLESMATTER_K8E
#define GP_COMP_JUMPTABLES_K8E
#define GP_COMP_LOG
#define GP_COMP_MACCORE
#define GP_COMP_MACDISPATCHER
#define GP_COMP_NVM
#define GP_COMP_PAD
#define GP_COMP_PD
#define GP_COMP_POOLMEM
#define GP_COMP_QORVOBLEHOST
#define GP_COMP_QVIO
#define GP_COMP_RADIO
#define GP_COMP_RANDOM
#define GP_COMP_RESET
#define GP_COMP_RXARBITER
#define GP_COMP_SCHED
#define GP_COMP_SILEXCRYPTOSOC
#define GP_COMP_UPGRADE
#define GP_COMP_UTILS
#define GP_COMP_VERSION
#define GP_COMP_WMRK

/*
 * Components numeric ids
 */

#define GP_COMPONENT_ID_APP                                                      1
#define GP_COMPONENT_ID_ASSERT                                                   29
#define GP_COMPONENT_ID_BASECOMPS                                                35
#define GP_COMPONENT_ID_BLE                                                      154
#define GP_COMPONENT_ID_BLEACTIVITYMANAGER                                       228
#define GP_COMPONENT_ID_BLEADDRESSRESOLVER                                       214
#define GP_COMPONENT_ID_BLEADVERTISER                                            215
#define GP_COMPONENT_ID_BLECOMPS                                                 216
#define GP_COMPONENT_ID_BLECONFIG                                                217
#define GP_COMPONENT_ID_BLECONNECTIONMANAGER                                     75
#define GP_COMPONENT_ID_BLEDATACHANNELRXQUEUE                                    218
#define GP_COMPONENT_ID_BLEDATACHANNELTXQUEUE                                    219
#define GP_COMPONENT_ID_BLEDATACOMMON                                            220
#define GP_COMPONENT_ID_BLEDATARX                                                221
#define GP_COMPONENT_ID_BLEDATATX                                                222
#define GP_COMPONENT_ID_BLEINITIATOR                                             223
#define GP_COMPONENT_ID_BLELLCP                                                  224
#define GP_COMPONENT_ID_BLELLCPFRAMEWORK                                         225
#define GP_COMPONENT_ID_BLELLCPPROCEDURES                                        226
#define GP_COMPONENT_ID_BLEPRESCHED                                              234
#define GP_COMPONENT_ID_BLESECURITYCOPROCESSOR                                   229
#define GP_COMPONENT_ID_BSP                                                      8
#define GP_COMPONENT_ID_COM                                                      10
#define GP_COMPONENT_ID_ECC                                                      192
#define GP_COMPONENT_ID_ENCRYPTION                                               124
#define GP_COMPONENT_ID_FREERTOS                                                 24
#define GP_COMPONENT_ID_GPHAL                                                    7
#define GP_COMPONENT_ID_HALCORTEXM4                                              6
#define GP_COMPONENT_ID_HCI                                                      156
#define GP_COMPONENT_ID_JUMPTABLESMATTER_K8E                                     60
#define GP_COMPONENT_ID_JUMPTABLES_K8E                                           60
#define GP_COMPONENT_ID_LOG                                                      11
#define GP_COMPONENT_ID_MACCORE                                                  109
#define GP_COMPONENT_ID_MACDISPATCHER                                            114
#define GP_COMPONENT_ID_NVM                                                      32
#define GP_COMPONENT_ID_PAD                                                      126
#define GP_COMPONENT_ID_PD                                                       104
#define GP_COMPONENT_ID_POOLMEM                                                  106
#define GP_COMPONENT_ID_QORVOBLEHOST                                             185
#define GP_COMPONENT_ID_QVIO                                                     18
#define GP_COMPONENT_ID_RADIO                                                    204
#define GP_COMPONENT_ID_RANDOM                                                   108
#define GP_COMPONENT_ID_RESET                                                    33
#define GP_COMPONENT_ID_RXARBITER                                                2
#define GP_COMPONENT_ID_SCHED                                                    9
#define GP_COMPONENT_ID_SILEXCRYPTOSOC                                           54
#define GP_COMPONENT_ID_STAT                                                     22
#define GP_COMPONENT_ID_UPGRADE                                                  115
#define GP_COMPONENT_ID_UTILS                                                    4
#define GP_COMPONENT_ID_VERSION                                                  129
#define GP_COMPONENT_ID_WMRK                                                     51

/*
 * Component: gpBaseComps
 */

#define GP_BASECOMPS_DIVERSITY_NO_GPCOM_INIT
#define GP_BASECOMPS_DIVERSITY_NO_GPLOG_INIT
#define GP_BASECOMPS_DIVERSITY_NO_GPSCHED_INIT

/*
 * Component: gpBleAddressResolver
 */

#define GP_ROM_PATCHED_Ble_ClearWhitelist
#define GP_ROM_PATCHED_Ble_ManipulateWhiteListAllowedChecker
#define GP_ROM_PATCHED_Ble_ManipulateWhiteListChecker
#define GP_ROM_PATCHED_gpBleAddressResolver_EnableConnectedDevicesInWhiteList
#define GP_ROM_PATCHED_gpBleAddressResolver_UpdateWhiteListEntryState
#define GP_ROM_PATCHED_gpBleAddressResolver_UpdateWhiteListEntryStateBulk
#define GP_ROM_PATCHED_gpBle_LeAddDeviceToWhiteList
#define GP_ROM_PATCHED_gpBle_LeRemoveDeviceFromWhiteList

/*
 * Component: gpBleComps
 */

#define GP_DIVERSITY_BLE_ACL_CONNECTIONS_SUPPORTED
#define GP_DIVERSITY_BLE_CONNECTIONS_SUPPORTED
#define GP_DIVERSITY_BLE_LEGACY_ADVERTISING_FEATURE_PRESENT

/*
 * Component: gpBsp
 */

#define GP_DIVERSITY_QPG6105DK_B01

/*
 * Component: gpCom
 */

#define GP_COM_DIVERSITY_ACTIVATE_TX_CALLBACK
#define GP_COM_DIVERSITY_SERIAL_NO_SYN_NO_CRC
#define GP_COM_DIVERSITY_SERIAL_NO_SYN_SENDTO_ID                                 18
#define GP_COM_DIVERSITY_SERIAL_SYN_DISABLED

/*
 * Component: gpEncryption
 */

#define GP_ENCRYPTION_DIVERSITY_USE_AES_MMO_HW

/*
 * Component: gpFreeRTOS
 */

#define GP_DIVERSITY_FREERTOS
#define GP_FREERTOS_DIVERSITY_STATIC_ALLOC
#define GP_FREERTOS_DIVERSITY_USE_NON_DEFAULT_CONFIG
#define GP_FREERTOS_DIVERSITY_USE_OWN_CONFIG_HOOKS

/*
 * Component: gphal
 */

#define GP_COMP_GPHAL_ES
#define GP_COMP_GPHAL_ES_ABS_EVENT
#define GP_COMP_GPHAL_ES_EXT_EVENT
#define GP_COMP_GPHAL_ES_EXT_EVENT_WKUP
#define GP_COMP_GPHAL_ES_REL_EVENT
#define GP_DIVERSITY_GPHAL_INTERN
#define GP_DIVERSITY_GPHAL_K8E
#define GP_DIVERSITY_GPHAL_OSCILLATOR_BENCHMARK
#define GP_DIVERSITY_GPHAL_RADIO_MGMT_SUPPORTED
#define GP_DIVERSITY_RT_SYSTEM_IN_ROM
#define GP_DIVERSITY_RT_SYSTEM_MACFILTER_IN_ROM
#define GP_DIVERSITY_RT_SYSTEM_PARTS_IN_ROM
#define GP_HAL_DIVERSITY_INCLUDE_IPC
#define GP_HAL_DIVERSITY_SEC_CRYPTOSOC

/*
 * Component: gpHci
 */

#define GP_HCI_DIVERSITY_HOST_SERVER

/*
 * Component: gpJumpTables_k8e
 */

#define GP_DIVERSITY_JUMPTABLES
#define GP_DIVERSITY_JUMP_TABLE_ASSEMBLY

/*
 * Component: gpLog
 */

#define GP_LOG_DIVERSITY_NO_TIME_NO_COMPONENT_ID
#define GP_LOG_DIVERSITY_VSNPRINTF

/*
 * Component: gpMacCore
 */

#define GP_MACCORE_DIVERSITY_ASSOCIATION_ORIGINATOR
#define GP_MACCORE_DIVERSITY_ASSOCIATION_RECIPIENT
#define GP_MACCORE_DIVERSITY_FFD
#define GP_MACCORE_DIVERSITY_INDIRECT_TRANSMISSION
#define GP_MACCORE_DIVERSITY_POLL_ORIGINATOR
#define GP_MACCORE_DIVERSITY_POLL_RECIPIENT
#define GP_MACCORE_DIVERSITY_RAW_FRAMES
#define GP_MACCORE_DIVERSITY_SCAN_ACTIVE_ORIGINATOR
#define GP_MACCORE_DIVERSITY_SCAN_ACTIVE_RECIPIENT
#define GP_MACCORE_DIVERSITY_SCAN_ED_ORIGINATOR
#define GP_MACCORE_DIVERSITY_SCAN_ORIGINATOR
#define GP_MACCORE_DIVERSITY_SCAN_ORPHAN_ORIGINATOR
#define GP_MACCORE_DIVERSITY_SCAN_ORPHAN_RECIPIENT
#define GP_MACCORE_DIVERSITY_SCAN_RECIPIENT

/*
 * Component: gpMacDispatcher
 */

#define GP_MACDISPATCHER_DIVERSITY_SINGLE_STACK_FUNCTIONS

/*
 * Component: gpNvm
 */

#define GP_DATA_SECTION_NAME_NVM                                                 gpNvm
#define GP_DATA_SECTION_START_NVM                                                -0x4000
#define GP_DIVERSITY_NVM
#define GP_NVM_DIVERSITY_ELEMENT_IF
#define GP_NVM_DIVERSITY_ELEMIF_KEYMAP
#define GP_NVM_DIVERSITY_SUBPAGEDFLASH_V2
#define GP_NVM_DIVERSITY_TAG_IF
#define GP_NVM_DIVERSITY_USE_POOLMEM
#define GP_NVM_DIVERSITY_VARIABLE_SETTINGS
#define GP_NVM_DIVERSITY_VARIABLE_SIZE
#define GP_NVM_USE_ASSERT_SAFETY_NET

/*
 * Component: gpPd
 */

#define GP_DIVERSITY_PD_USE_PBM_VARIANT

/*
 * Component: gpRadio
 */

#define GP_RADIO_DIVERSITY_ENABLE_MULTISTANDARD_LISTENING_MODE

/*
 * Component: gpSched
 */

#define GP_ROM_PATCHED_Sched_CanGoToSleep
#define GP_ROM_PATCHED_Sched_DumpEvent
#define GP_ROM_PATCHED_Sched_ExecEvent
#define GP_ROM_PATCHED_Sched_FindEventArg
#define GP_ROM_PATCHED_Sched_GetEvent
#define GP_ROM_PATCHED_Sched_GetEventIdlePeriod
#define GP_ROM_PATCHED_Sched_ReleaseEvent
#define GP_ROM_PATCHED_Sched_ReleaseEventBody
#define GP_ROM_PATCHED_Sched_RescheduleEvent
#define GP_ROM_PATCHED_Sched_RescheduleEventAbs
#define GP_ROM_PATCHED_Sched_ScheduleEvent
#define GP_ROM_PATCHED_Sched_ScheduleEventInSeconds
#define GP_ROM_PATCHED_gpSched_Clear
#define GP_ROM_PATCHED_gpSched_DeInit
#define GP_ROM_PATCHED_gpSched_DumpList
#define GP_ROM_PATCHED_gpSched_EventQueueEmpty
#define GP_ROM_PATCHED_gpSched_ExistsEventArg
#define GP_ROM_PATCHED_gpSched_GetRemainingTimeArgInSecAndUs
#define GP_ROM_PATCHED_gpSched_GoToSleep
#define GP_ROM_PATCHED_gpSched_Init
#define GP_ROM_PATCHED_gpSched_Main_Body
#define GP_ROM_PATCHED_gpSched_ScheduleEventArg
#define GP_ROM_PATCHED_gpSched_SetGotoSleepEnable
#define GP_ROM_PATCHED_gpSched_UnscheduleEventArg
#define GP_SCHED_DIVERSITY_SCHEDULE_INSECONDSAPI
#define GP_SCHED_DIVERSITY_USE_ARGS

/*
 * Component: gpUpgrade
 */

#define GP_APP_DIVERSITY_SECURE_BOOTLOADER
#define GP_DATA_SECTION_NAME_OTA                                                 OTA
#define GP_DATA_SECTION_SIZE_OTA                                                 0x5c000
#define GP_DATA_SECTION_START_OTA                                                -0x60000
#define GP_DIVERSITY_APP_LICENSE_BASED_BOOT
#define GP_DIVERSITY_FLASH_APP_START_OFFSET                                      0x6000
#define GP_UPGRADE_DIVERSITY_COMPRESSION

/*
 * Component: gpUtils
 */

#define GP_DIVERSITY_UTILS_MATH
#define GP_UTILS_DIVERSITY_CIRCULAR_BUFFER
#define GP_UTILS_DIVERSITY_LINKED_LIST

/*
 * Component: halCortexM4
 */

#define GP_DIVERSITY_ENABLE_DEFAULT_BOD_HANDLING
#define GP_DIVERSITY_FLASH_BL_SIZE                                               0x2500
#define GP_DIVERSITY_ROMUSAGE_FOR_MATTER
#define GP_KX_FLASH_SIZE                                                         1024
#define GP_KX_HEAP_SIZE                                                          (4 * 1024)
#define GP_KX_SYSRAM_SIZE                                                        32
#define GP_KX_UCRAM_SIZE                                                         96
#define HAL_DIVERSITY_UART
#define HAL_DIVERSITY_UART_RX_BUFFER_CALLBACK
#define QPG6105

/*
 * Component: qorvoBleHost
 */

#define CORDIO_BLEHOST_DIVERSITY_HCI_INTERNAL
#define CORDIO_BLE_HOST_ATT_SERVER
#define CORDIO_BLE_HOST_EXCLUDE_CORDIOAPPFW
#define CORDIO_BLE_HOST_EXCLUDE_SMPR
#define CORDIO_BLE_HOST_PROFILES_ORIG_SERVPROF
#define WSF_ASSERT_ENABLED                                                       TRUE

/*
 * Component: QorvoStack
 */

#define GP_DATA_SECTION_NAME_FACTORYDATA                                         factory_data
#define GP_DATA_SECTION_NAME_JTOTA                                               JTOTA
#define GP_DATA_SECTION_SIZE_FACTORYDATA                                         0x800
#define GP_DATA_SECTION_SIZE_JTOTA                                               0x1000
#define GP_DATA_SECTION_START_FACTORYDATA                                        0x4004800

/*
 * Component: silexCryptoSoc
 */

#define GP_ROM_PATCHED_ba414e_set_config
#define GP_ROM_PATCHED_generate_ccm_header
#define GP_SILEXCRYPTOSOC_DIVERSITY_ECC_CURVES_IN_FLASH

/*
 * Other flags
 */

#define GP_DATA_SECTION_START_JTOTA                                              -0x61000
#define GP_DIVERSITY_BOOTLOADER
#define GP_DIVERSITY_CORTEXM4
#define GP_DIVERSITY_GPHAL_INDIRECT_TRANSMISSION
#define GP_DIVERSITY_KEEP_NRT_FROM_FPGABITMAP
#define GP_DIVERSITY_LOG
#define GP_DIVERSITY_NR_OF_STACKS                                                1
#define GP_GIT_SHA                                                               2469438626c13beea2391947d34e05a62039041a
#define GP_GIT_SHA_SHORT                                                         2469438
#define GP_HAL_ES_ABS_EVENT_NMBR_OF_EVENTS                                       10
#define GP_LINKER_RESERVED_SECTIONS_PRIO_LIST                                    NVM,OTA,JTOTA
#define GP_POOLMEM_DIVERSITY_MALLOC
#define HAL_DIVERSITY_PWM
#define HAL_DIVERSITY_SPI
#define HAL_DIVERSITY_TWI
#define HAL_TWI_CLK_SPEED                                                        100000

#endif //_QORVO_INTERNALS_H_
