/*******************************************************************************
* @file  rsi_user.h
* @brief 
*******************************************************************************
* # License
* <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
*******************************************************************************
*
* The licensor of this software is Silicon Laboratories Inc. Your use of this
* software is governed by the terms of Silicon Labs Master Software License
* Agreement (MSLA) available at
* www.silabs.com/about-us/legal/master-software-license-agreement. This
* software is distributed to you in Source Code format and is governed by the
* sections of the MSLA applicable to Source Code.
*
******************************************************************************/

#ifndef RSI_USER_H
#define RSI_USER_H
/******************************************************
 * *                      Macros
 * ******************************************************/

// Silicon Version 1.3
#define CHIP_VER_1P3 0

// Silicon Version 1.4 and above
#define CHIP_VER_1P4_AND_ABOVE 1

// To selcet Silicon version, select either of one from among two macros
#define RS9116_SILICON_CHIP_VER \
  CHIP_VER_1P4_AND_ABOVE // Set 1 for Silicon Chip Version 1p4 and Above, 0 for Silicon Chip Version 1p3

// For TASK Notify
#define RSI_TASK_NOTIFY 0

#define WIFI_INTERNAL_TIMEOUT_SF 1
#define WIFI_WAIT_TIMEOUT_SF     1
#define WIFI_BLOCKED_TIMEOUT_SF  1
#define TX_WAIT_TIME             0

#define BT_WAIT_TIMEOUT_SF  1
#define BLE_WAIT_TIMEOUT_SF 1
#ifdef RSI_PROP_PROTOCOL_ENABLE
#define PROP_PROTOCOL_WAIT_TIMEOUT_SF 1
#endif
#define BT_COMMON_WAIT_TIMEOUT_SF 1

//! No of ltcp socktes
//#define RSI_NUMBER_OF_LTCP_SOCKETS 2
#define RSI_NUMBER_OF_LTCP_SOCKETS 0

//! Default number of sockets supported,max 10 sockets are supported
//#define RSI_NUMBER_OF_SOCKETS (2 + RSI_NUMBER_OF_LTCP_SOCKETS)
#define RSI_NUMBER_OF_SOCKETS (6 + RSI_NUMBER_OF_LTCP_SOCKETS)

//! Default number of sockets supported,max 10 selects are supported
//#define RSI_NUMBER_OF_SELECTS 4
#define RSI_NUMBER_OF_SELECTS (RSI_NUMBER_OF_SOCKETS)

// Max packets in wlan control tx pool
#define RSI_WLAN_TX_POOL_PKT_COUNT 1

// Max packets in bt common control tx pool
#define RSI_BT_COMMON_TX_POOL_PKT_COUNT 1

// Max packets in bt classic control tx pool
#define RSI_BT_CLASSIC_TX_POOL_PKT_COUNT 1

// Max packets in ble control tx pool
#define RSI_BLE_TX_POOL_PKT_COUNT 1

#ifdef RSI_PROP_PROTOCOL_ENABLE
// User configured MAX number of PROP_PROTOCOL TX PKTS pool COUNT.
#define PROP_PROTOCOL_TX_POOL_CONFIG_VAL 5

// Max packets in PROP_PROTOCOL control tx pool
#define RSI_PROP_PROTOCOL_TX_POOL_PKT_COUNT PROP_PROTOCOL_TX_POOL_CONFIG_VAL
#endif

// Max packets in wlan control tx pool
#define RSI_ZIGB_TX_POOL_PKT_COUNT 1

// Max packets in common control tx pool
#define RSI_COMMON_TX_POOL_PKT_COUNT 1

// Max packets

#define RSI_DRIVER_RX_POOL_PKT_COUNT 1

// Little Endian is enabled
#define RSI_LITTLE_ENDIAN 1

// Maximum scan results to be stored
#define MAX_SCAN_COUNT 48

// HAL specific maximum write buffer length in bytes
#define RSI_HAL_MAX_WR_BUFF_LEN 4096

#ifdef SAPI_LOGGING_ENABLE
#define CONFIG_DEBUG_LEVEL        LOG_ERROR
#define LOG_TSF_GRANULARITY_TICKS 1UL
#endif

/******************************************************
 * *                    Constants
 * ******************************************************/
/******************************************************
 * *                   Enumerations
 * ******************************************************/
/******************************************************
 * *                 Type Definitions
 * ******************************************************/
/******************************************************
 * *                    Structures
 * ******************************************************/
/******************************************************
 * *                 Global Variables
 * ******************************************************/
/******************************************************
 * *               Function Declarations
 * ******************************************************/
#endif
