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

/** @file "qorvo_config.h"
 *
 */

#ifndef _QORVO_CONFIG_H_
#define _QORVO_CONFIG_H_

/*
 * Version info
 */

#define GP_CHANGELIST                                                            0
#define GP_VERSIONINFO_APP                                                       base_qpg6105
#define GP_VERSIONINFO_BASE_COMPS                                                0,0,0,0
#define GP_VERSIONINFO_BLE_COMPS                                                 2,10,2,8
#define GP_VERSIONINFO_DATE                                                      2022-12-07
#define GP_VERSIONINFO_GLOBAL_VERSION                                            0,0,0,0
#define GP_VERSIONINFO_HOST                                                      UNKNOWN
#define GP_VERSIONINFO_PROJECT                                                   P345_Matter_DK_Endnodes
#define GP_VERSIONINFO_USER                                                      UNKNOWN@UNKNOWN


/*
 * Component: base
 */

#define AES_GCM_EMABLED                                                          0

#define AES_HW_KEYS_ENABLED                                                      0

#define AES_MASK_ENABLED                                                         0

/* The MTU size */
#define CORDIO_BLE_HOST_ATT_MAX_MTU                                              23

/* MTU is configured at runtime */
#define CORDIO_BLE_HOST_ATT_MTU_CONFIG_AT_RUNTIME

/* Number of chuncks WSF Poolmem Chunk 1 */
#define CORDIO_BLE_HOST_BUFPOOLS_1_AMOUNT                                        3

/* Number of chuncks WSF Poolmem Chunk 2 */
#define CORDIO_BLE_HOST_BUFPOOLS_2_AMOUNT                                        4

/* Number of chuncks WSF Poolmem Chunk 3 */
#define CORDIO_BLE_HOST_BUFPOOLS_3_AMOUNT                                        2

/* Number of chuncks WSF Poolmem Chunk 4 */
#define CORDIO_BLE_HOST_BUFPOOLS_4_AMOUNT                                        2

/* Number of chuncks WSF Poolmem Chunk 5 */
#define CORDIO_BLE_HOST_BUFPOOLS_5_AMOUNT                                        2

/* Number of WSF Poolmem Chunks in use */
#define CORDIO_BLE_HOST_WSF_BUF_POOLS                                            5

/* Cordio define - checked here */
#define DM_CONN_MAX                                                              1

/* Amount of 64-bit long IEEE addresses entries to keep data pending for */
#define GPHAL_DP_LONG_LIST_MAX                                                   10

/* Amount of 16-bit short address entries to keep data pending for */
#define GPHAL_DP_SHORT_LIST_MAX                                                  10

/* Buffer size for HCI RX packets */
#define GP_BLE_DIVERSITY_HCI_BUFFER_SIZE_RX                                      30

/* Buffer size for HCI TX packets */
#define GP_BLE_DIVERSITY_HCI_BUFFER_SIZE_TX                                      30

/* Contains filename of BSP header file to include */
#define GP_BSP_FILENAME                                                          "gpBsp_QPG6105DK_B01.h"

/* UART baudrate */
#define GP_BSP_UART_COM_BAUDRATE                                                 115200

/* Multiple gpComs were specified - defined in code */
#define GP_COM_DIVERSITY_MULTIPLE_COM

/* Enable SYN datastream encapsulation */
#define GP_COM_DIVERSITY_SERIAL

/* Maximum amount of modules supported for Rx handling by gpCom. Environment already calculates minimal required module ID's */
#define GP_COM_MAX_NUMBER_OF_MODULE_IDS                                          2

#define GP_COM_MAX_PACKET_PAYLOAD_SIZE                                           250

/* Size of reserved section for NVM */
#define GP_DATA_SECTION_SIZE_NVM                                                 0x4000

/* Support for A25L080 SPI flash chip */
#define GP_DIVERSITY_A25L080_SPIFLASH

/* Legacy advertiser functionality */
#define GP_DIVERSITY_BLE_ADVERTISER

/* WcBleHost will be calling gpBle_ExecuteCommand */
#define GP_DIVERSITY_BLE_EXECUTE_CMD_WCBLEHOST

/* Max BLE connections supported */
#define GP_DIVERSITY_BLE_MAX_NR_OF_SUPPORTED_CONNECTIONS                         1

/* Max BLE slave connections supported */
#define GP_DIVERSITY_BLE_MAX_NR_OF_SUPPORTED_SLAVE_CONNECTIONS                   1

/* Number of entries in the whitelist */
#define GP_DIVERSITY_BLE_MAX_NR_OF_WHITELIST_ENTRIES                             1

/* BLE Slave functionality */
#define GP_DIVERSITY_BLE_SLAVE

/* Bootloader added to SW */
#define GP_DIVERSITY_BOOTLOADER

/* Use UART for COM - defined as default in code */
#define GP_DIVERSITY_COM_UART

/* Use extended loaded user license */
#define GP_DIVERSITY_EXTENDED_USER_LICENSE

/* Use loaded user license */
#define GP_DIVERSITY_LOADED_USER_LICENSE

/* Enable logging */
#define GP_DIVERSITY_LOG

/* The number of stacks that are located in the maccore */
#define GP_DIVERSITY_NR_OF_STACKS                                                1

/* gp scheduler ROM ver 2.0 */
#define GP_DIVERSITY_ROM_GPSCHED_V2

/* Enable Multistandard Listening: support 802.15.4 RxOnWhenIdle + BLE Observer/Central */
#define GP_HAL_DIVERSITY_MULTISTANDARD_LISTENING_MODE

/* Do CSMA-CA in software */
#define GP_HAL_MAC_SW_CSMA_CA

/* Number of PBMS of first supported size */
#define GP_HAL_PBM_TYPE1_AMOUNT                                                  12

/* Number of PBMS of second supported size */
#define GP_HAL_PBM_TYPE2_AMOUNT                                                  8

/* set custom stack size */
#define GP_KX_STACK_SIZE                                                         512

/* overrule log string length from application code */
#define GP_LOG_MAX_LEN                                                           256

/* Number of known Neighbours for use with indirect transmission */
#define GP_MACCORE_MAX_NEIGHBOURS                                                10

/* maximal length of a token used */
#define GP_NVM_MAX_TOKENLENGTH                                                   13

/* set working range of gpNvm_PoolId_t, requires setting number of phy sectors for each pool */
#define GP_NVM_NBR_OF_POOLS                                                      1

/* Maximum number of unique tags in each pool. Used for memory allocation at Tag level API */
#define GP_NVM_NBR_OF_UNIQUE_TAGS                                                23

/* Maximum number of tokens tracked by token API */
#define GP_NVM_NBR_OF_UNIQUE_TOKENS                                              200

/* number of sectors of pool 1 */
#define GP_NVM_POOL_1_NBR_OF_PHY_SECTORS                                         16

#define GP_NVM_TYPE                                                              6

/* Number of packet descriptors */
#define GP_PD_NR_OF_HANDLES                                                      20

/*  dynamic heap memory usage */
#define GP_POOLMEM_DIVERSITY_MALLOC

#define GP_SCHED_EVENT_LIST_SIZE                                                 10

/* Don't include the implementation for our mainloop MAIN_FUNCTION_NAME */
#define GP_SCHED_EXTERNAL_MAIN

/* Include curve, optimization diversity, to exclude other curves from taking up flash space */
#define GP_SILEXCRYPTOSOC_DIVERSITY_INCLUDE_SX_ECP_DP_SECP256R1

/* Don't use gpExtStorage, even if available */
#define GP_UPGRADE_DIVERSITY_USE_INTSTORAGE

/* Select GPIO level interrupt code */
#define HAL_DIVERSITY_GPIO_INTERRUPT

/* Include SPI support */
#define HAL_DIVERSITY_SPI

/* Set if hal has real mutex capability. Used to skip even disabling/enabling global interrupts. */
#define HAL_MUTEX_SUPPORTED

/* Clk speed of the TWI in Hz */
#define HAL_TWI_CLK_SPEED                                                        100000

/* Change the name of the main eventloop implementation */
#define MAIN_FUNCTION_NAME                                                       main


/*
 * Component: gpBleComps
 */

/* The amount of dedicated connection complete buffers */
#define GP_BLE_NR_OF_CONNECTION_COMPLETE_EVENT_BUFFERS                           0


/*
 * Component: gpSched
 */

/* Callback after every main loop iteration. */
#define GP_SCHED_NR_OF_IDLE_CALLBACKS                                            0


#include "qorvo_internals.h"

#endif //_QORVO_CONFIG_H_
