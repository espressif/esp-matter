/*******************************************************************************
* @file  rsi_ble_common_config.h
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
/**
 * @file         rsi_ble_common_config.h
 * @version      0.1
 * @date         15 Aug 2018
 *
 *
 *
 *  @brief : This file contains user configurable details to configure the device  
 *
 *  @section Description  This file contains user configurable details to configure the device 
 *
 *
 */
#ifndef RSI_BLE_COMMON_CONFIG_H
#define RSI_BLE_COMMON_CONFIG_H
#include <rsi_ble_apis.h>
#include "rsi_data_types.h"

#ifndef RSI_BLE_SET_RESOLVABLE_PRIV_ADDR_TOUT
#define RSI_BLE_SET_RESOLVABLE_PRIV_ADDR_TOUT 120
#endif
#ifndef NO_OF_ADV_REPORTS
#define NO_OF_ADV_REPORTS 10 // Maximum number of advertise reports to hold
#endif
/*=======================================================================*/
// attribute properties
/*=======================================================================*/
#ifndef RSI_BLE_ATT_PROPERTY_READ
#define RSI_BLE_ATT_PROPERTY_READ 0x02
#endif
#ifndef RSI_BLE_ATT_PROPERTY_WRITE_NO_RESPONSE
#define RSI_BLE_ATT_PROPERTY_WRITE_NO_RESPONSE 0x04
#endif
#ifndef RSI_BLE_ATT_PROPERTY_WRITE
#define RSI_BLE_ATT_PROPERTY_WRITE 0x08
#endif
#ifndef RSI_BLE_ATT_PROPERTY_NOTIFY
#define RSI_BLE_ATT_PROPERTY_NOTIFY 0x10
#endif
#ifndef RSI_BLE_ATT_PROPERTY_INDICATE
#define RSI_BLE_ATT_PROPERTY_INDICATE 0x20
#endif

#ifndef BT_GLOBAL_BUFF_LEN
#define BT_GLOBAL_BUFF_LEN 10000 // Memory length for driver
#endif

#ifndef BLE_PERIPHERAL_MODE_ENABLE
#define BLE_PERIPHERAL_MODE_ENABLE 1 // Enabling peripheral mode
#endif
/*=======================================================================*/
// BLE Attribute Security Define
/*=======================================================================*/
#define ATT_REC_MAINTAIN_IN_HOST BIT(0) /* Att record maintained by the stack */
#define SEC_MODE_1_LEVEL_1       BIT(1) /* NO Auth & No Enc                   */
#define SEC_MODE_1_LEVEL_2       BIT(2) /* UnAUTH with Enc                    */
#define SEC_MODE_1_LEVEL_3       BIT(3) /* AUTH with Enc                      */
#define SEC_MODE_1_LEVEL_4       BIT(4) /* AUTH LE_SC Pairing with Enc        */
#define ON_BR_EDR_LINK_ONLY      BIT(5)
#define ON_LE_LINK_ONLY          BIT(6)
/*=======================================================================*/
// Max Tx data length and Tx timeout values
/*=======================================================================*/
#ifndef TX_LEN
#define TX_LEN 0x001e
#endif
#ifndef TX_TIME
#define TX_TIME 0x01f4
#endif
#ifndef RSI_BLE_SMP_IO_CAPABILITY
#define RSI_BLE_SMP_IO_CAPABILITY 0x00
#endif
#ifndef RSI_BLE_APP_SMP_PASSKEY
#define RSI_BLE_APP_SMP_PASSKEY 0
#endif
/*=======================================================================*/
// PHY rate selections
/*=======================================================================*/
#ifndef TX_PHY_RATE
#define TX_PHY_RATE 0x02
#endif
#ifndef RX_PHY_RATE
#define RX_PHY_RATE 0x02
#endif
#ifndef CODDED_PHY_RATE
#define CODDED_PHY_RATE 0x00
#endif

/*=======================================================================*/
// privacy mode 0-Network privacy mode 1-Device privacy mode
/*=======================================================================*/
#ifndef RSI_BLE_NETWORK_PRIVACY_MODE
#define RSI_BLE_NETWORK_PRIVACY_MODE 0
#endif
#ifndef RSI_BLE_DEVICE_PRIVACY_MODE
#define RSI_BLE_DEVICE_PRIVACY_MODE 1
#endif
#ifndef RSI_BLE_PRIVACY_MODE
#define RSI_BLE_PRIVACY_MODE RSI_BLE_DEVICE_PRIVACY_MODE
#endif

#ifndef RSI_BLE_RESOLVING_LIST_SIZE
#define RSI_BLE_RESOLVING_LIST_SIZE 5 // Resovable list size
#endif
#ifndef RSI_BLE_MAX_DATA_LEN
#define RSI_BLE_MAX_DATA_LEN 20 // max data length
#endif
/*=======================================================================*/
// process type 1-add device to resolvlist, 2-remove device from resolvlist, 3-clear the resolvlist
/*=======================================================================*/
#ifndef RSI_BLE_ADD_TO_RESOLVE_LIST
#define RSI_BLE_ADD_TO_RESOLVE_LIST 1
#endif
#ifndef RSI_BLE_REMOVE_FROM_RESOLVE_LIST
#define RSI_BLE_REMOVE_FROM_RESOLVE_LIST 2
#endif
#ifndef RSI_BLE_CLEAR_RESOLVE_LIST
#define RSI_BLE_CLEAR_RESOLVE_LIST 3
#endif
/*=======================================================================*/
// BLE attribute service types uuid values
/*=======================================================================*/
#ifndef RSI_BLE_BATTERY_SERVICE_UUID
#define RSI_BLE_BATTERY_SERVICE_UUID 0x180F
#endif
#ifndef RSI_BLE_CHAR_SERV_UUID
#define RSI_BLE_CHAR_SERV_UUID 0x2803
#endif
#ifndef RSI_BLE_CLIENT_CHAR_UUID
#define RSI_BLE_CLIENT_CHAR_UUID 0x2902
#endif
#ifndef RSI_BLE_CHAR_PRESENTATION_FORMATE_UUID
#define RSI_BLE_CHAR_PRESENTATION_FORMATE_UUID 0x2904
#endif
/*=======================================================================*/
// BLE characteristic service and attribute uuid
/*=======================================================================*/
#ifndef RSI_BLE_NEW_SERVICE_UUID
#define RSI_BLE_NEW_SERVICE_UUID 0xAABB
#endif
#ifndef RSI_BLE_NEW_SERVICE_UUID2
#define RSI_BLE_NEW_SERVICE_UUID2 0xCCDD
#endif
#ifndef RSI_BLE_ATTRIBUTE_1_UUID
#define RSI_BLE_ATTRIBUTE_1_UUID 0x1AA1
#endif
#ifndef RSI_BLE_ATTRIBUTE_2_UUID
#define RSI_BLE_ATTRIBUTE_2_UUID 0x1BB1
#endif
#ifndef RSI_BLE_ATTRIBUTE_3_UUID
#define RSI_BLE_ATTRIBUTE_3_UUID 0x1CC1
#endif
#ifndef RSI_BLE_ATTRIBUTE_4_UUID
#define RSI_BLE_ATTRIBUTE_4_UUID 0x1DD1
#endif

#ifndef RSI_BLE_SET_RAND_ADDR
#define RSI_BLE_SET_RAND_ADDR "00:23:A7:12:34:56"
#endif

#ifndef CLEAR_WHITELIST
#define CLEAR_WHITELIST 0x00
#endif
#ifndef ADD_DEVICE_TO_WHITELIST
#define ADD_DEVICE_TO_WHITELIST 0x01
#endif
#ifndef DELETE_DEVICE_FROM_WHITELIST
#define DELETE_DEVICE_FROM_WHITELIST 0x02
#endif

#ifndef ALL_PHYS
#define ALL_PHYS 0x00
#endif

#ifndef RSI_BLE_DEV_ADDR_RESOLUTION_ENABLE
#define RSI_BLE_DEV_ADDR_RESOLUTION_ENABLE 0
#endif

#ifndef RSI_OPERMODE_WLAN_BLE
#define RSI_OPERMODE_WLAN_BLE 13
#endif

#ifndef RSI_BLE_MAX_NBR_ATT_REC
#define RSI_BLE_MAX_NBR_ATT_REC 80
#endif
#ifndef RSI_BLE_MAX_NBR_ATT_SERV
#define RSI_BLE_MAX_NBR_ATT_SERV 10
#endif

#ifndef RSI_BLE_MAX_NBR_SLAVES
#define RSI_BLE_MAX_NBR_SLAVES 3
#endif
#ifndef RSI_BLE_MAX_NBR_MASTERS
#define RSI_BLE_MAX_NBR_MASTERS 2
#endif
#ifndef RSI_BLE_NUM_CONN_EVENTS
#define RSI_BLE_NUM_CONN_EVENTS 20
#endif

/* Number of BLE GATT RECORD SIZE IN (n*16 BYTES), eg:(0x40*16)=1024 bytes */
#ifndef RSI_BLE_NUM_REC_BYTES
#define RSI_BLE_NUM_REC_BYTES 0x40
#endif
/*=======================================================================*/
// Advertising command parameters
/*=======================================================================*/
#ifndef RSI_BLE_ADV_DIR_ADDR
#define RSI_BLE_ADV_DIR_ADDR "00:15:83:6A:64:17"
#endif

#ifndef RSI_BLE_ADV_CHANNEL_MAP
#define RSI_BLE_ADV_CHANNEL_MAP 0x07
#endif

// BLE Tx Power Index On Air
#ifndef RSI_BLE_PWR_INX
#define RSI_BLE_PWR_INX 30
#endif

// BLE Tx Power Index On Air per Gap role
#ifndef RSI_BLE_PWR_INX_DBM
#define RSI_BLE_PWR_INX_DBM 0
#endif

#ifndef RSI_BLE_START_ADV
#define RSI_BLE_START_ADV 0x01 // Start the advertising process
#endif
#ifndef RSI_BLE_STOP_ADV
#define RSI_BLE_STOP_ADV 0x00 // Stop the advertising process
#endif
/*=======================================================================*/
// Advertise type
/*=======================================================================*/
/* Advertising will be visible(discoverable) to all the devices.
 * Scanning/Connection is also accepted from all devices
 * */
#ifndef UNDIR_CONN
#define UNDIR_CONN 0x80
#endif
/* Advertising will be visible(discoverable) to the particular device 
 * mentioned in RSI_BLE_ADV_DIR_ADDR only. 
 * Scanning and Connection will be accepted from that device only.
 * */
#ifndef DIR_CONN
#define DIR_CONN 0x81
#endif
/* Advertising will be visible(discoverable) to all the devices.
 * Scanning will be accepted from all the devices.
 * Connection will be not be accepted from any device.
 * */
#ifndef UNDIR_SCAN
#define UNDIR_SCAN 0x82
#endif
/* Advertising will be visible(discoverable) to all the devices.
 * Scanning and Connection will not be accepted from any device
 * */
#ifndef UNDIR_NON_CONN
#define UNDIR_NON_CONN 0x83
#endif
/* Advertising will be visible(discoverable) to the particular device 
 * mentioned in RSI_BLE_ADV_DIR_ADDR only. 
 * Scanning and Connection will be accepted from that device only.
 * */
#ifndef DIR_CONN_LOW_DUTY_CYCLE
#define DIR_CONN_LOW_DUTY_CYCLE 0x84
#endif

#ifndef RSI_BLE_ADV_TYPE
#define RSI_BLE_ADV_TYPE UNDIR_CONN
#endif
/*=======================================================================*/
// Advertising flags
/*=======================================================================*/
#ifndef LE_LIMITED_DISCOVERABLE
#define LE_LIMITED_DISCOVERABLE 0x01
#endif
#ifndef LE_GENERAL_DISCOVERABLE
#define LE_GENERAL_DISCOVERABLE 0x02
#endif
#ifndef LE_BR_EDR_NOT_SUPPORTED
#define LE_BR_EDR_NOT_SUPPORTED 0x04
#endif
/*=======================================================================*/
// Advertise filters
/*=======================================================================*/
#ifndef ALLOW_SCAN_REQ_ANY_CONN_REQ_ANY
#define ALLOW_SCAN_REQ_ANY_CONN_REQ_ANY 0x00
#endif
#ifndef ALLOW_SCAN_REQ_WHITE_LIST_CONN_REQ_ANY
#define ALLOW_SCAN_REQ_WHITE_LIST_CONN_REQ_ANY 0x01
#endif
#ifndef ALLOW_SCAN_REQ_ANY_CONN_REQ_WHITE_LIST
#define ALLOW_SCAN_REQ_ANY_CONN_REQ_WHITE_LIST 0x02
#endif
#ifndef ALLOW_SCAN_REQ_WHITE_LIST_CONN_REQ_WHITE_LIST
#define ALLOW_SCAN_REQ_WHITE_LIST_CONN_REQ_WHITE_LIST 0x03
#endif

#ifndef RSI_BLE_ADV_FILTER_TYPE
#define RSI_BLE_ADV_FILTER_TYPE ALLOW_SCAN_REQ_ANY_CONN_REQ_ANY
#endif
/*=======================================================================*/
// Address types
/*=======================================================================*/
#ifndef LE_PUBLIC_ADDRESS
#define LE_PUBLIC_ADDRESS 0x00
#endif
#ifndef LE_RANDOM_ADDRESS
#define LE_RANDOM_ADDRESS 0x01
#endif
#ifndef LE_RESOLVABLE_PUBLIC_ADDRESS
#define LE_RESOLVABLE_PUBLIC_ADDRESS 0x02
#endif
#ifndef LE_RESOLVABLE_RANDOM_ADDRESS
#define LE_RESOLVABLE_RANDOM_ADDRESS 0x03
#endif

#ifndef RSI_BLE_ADV_DIR_ADDR_TYPE
#define RSI_BLE_ADV_DIR_ADDR_TYPE LE_PUBLIC_ADDRESS
#endif
/*=======================================================================*/
// Connection parameters
/*=======================================================================*/
#ifndef CONNECTION_LATENCY
#define CONNECTION_LATENCY 0x0000
#endif
#ifndef SUPERVISION_TIMEOUT
#define SUPERVISION_TIMEOUT 0x07D0 //2000
#endif

/*=======================================================================*/
// Scan command parameters
/*=======================================================================*/
//Scan status
#ifndef RSI_BLE_START_SCAN
#define RSI_BLE_START_SCAN 0x01
#endif
#ifndef RSI_BLE_STOP_SCAN
#define RSI_BLE_STOP_SCAN 0x00
#endif

//Scan types
#ifndef SCAN_TYPE_ACTIVE
#define SCAN_TYPE_ACTIVE 0x01
#endif
#ifndef SCAN_TYPE_PASSIVE
#define SCAN_TYPE_PASSIVE 0x00
#endif

//Scan filters
#ifndef SCAN_FILTER_TYPE_ALL
#define SCAN_FILTER_TYPE_ALL 0x00
#endif
#ifndef SCAN_FILTER_TYPE_ONLY_WHITE_LIST
#define SCAN_FILTER_TYPE_ONLY_WHITE_LIST 0x01
#endif

#ifndef RSI_BLE_SCAN_TYPE
#define RSI_BLE_SCAN_TYPE SCAN_TYPE_ACTIVE
#endif

#ifndef RSI_BLE_SCAN_FILTER_TYPE
#define RSI_BLE_SCAN_FILTER_TYPE SCAN_FILTER_TYPE_ALL
#endif
/*-------------------------------------------------------------------------------------------*/
#ifndef BLE_DISABLE_DUTY_CYCLING
#define BLE_DISABLE_DUTY_CYCLING 0
#endif
#ifndef BLE_DUTY_CYCLING
#define BLE_DUTY_CYCLING 1
#endif
#ifndef BLR_DUTY_CYCLING
#define BLR_DUTY_CYCLING 2
#endif
#ifndef BLE_4X_PWR_SAVE_MODE
#define BLE_4X_PWR_SAVE_MODE 4
#endif
#ifndef RSI_BLE_PWR_SAVE_OPTIONS
#define RSI_BLE_PWR_SAVE_OPTIONS BLE_DISABLE_DUTY_CYCLING
#endif
/*-------------------------------------------------------------------------------------------*/
#ifndef RSI_SEL_INTERNAL_ANTENNA
#define RSI_SEL_INTERNAL_ANTENNA 0x00
#endif
#ifndef RSI_SEL_EXTERNAL_ANTENNA
#define RSI_SEL_EXTERNAL_ANTENNA 0x01
#endif
#endif

#ifndef RSI_SEL_ANTENNA
#define RSI_SEL_ANTENNA \
  RSI_SEL_INTERNAL_ANTENNA // Antenna Selection - RSI_SEL_EXTERNAL_ANTENNA / RSI_SEL_INTERNAL_ANTENNA
#endif
/*-------------------------------------------------------------------------------------------*/
#ifndef RSI_BLE_GATT_ASYNC_ENABLE
#define RSI_BLE_GATT_ASYNC_ENABLE 0
#endif

#ifndef RSI_BLE_GATT_INIT
#define RSI_BLE_GATT_INIT 0
#endif

#ifndef RSI_BLE_INDICATE_CONFIRMATION_FROM_HOST
#define RSI_BLE_INDICATE_CONFIRMATION_FROM_HOST 0
#endif

#ifndef RSI_BLE_MTU_EXCHANGE_FROM_HOST
#define RSI_BLE_MTU_EXCHANGE_FROM_HOST 0
#endif

// If user wants to set scan resp data from Application this BIT has to set.
#ifndef RSI_BLE_SET_SCAN_RESP_DATA_FROM_HOST
#define RSI_BLE_SET_SCAN_RESP_DATA_FROM_HOST 0
#endif

#ifndef RSI_BLE_DISABLE_CODED_PHY_FROM_HOST
#define RSI_BLE_DISABLE_CODED_PHY_FROM_HOST 0
#endif
/*-------------------------------------------------------------------------------------------*/
// Advertising command parameters
#ifndef RSI_BLE_ADV_INT_MIN
#define RSI_BLE_ADV_INT_MIN 0x100
#endif
#ifndef RSI_BLE_ADV_INT_MAX
#define RSI_BLE_ADV_INT_MAX 0x200
#endif

#ifndef CONNECTION_INTERVAL_MIN
#define CONNECTION_INTERVAL_MIN 0x00A0
#endif
#ifndef CONNECTION_INTERVAL_MAX
#define CONNECTION_INTERVAL_MAX 0x00A0
#endif

#ifndef LE_SCAN_INTERVAL
#define LE_SCAN_INTERVAL 0x0100
#endif
#ifndef LE_SCAN_WINDOW
#define LE_SCAN_WINDOW 0x0050
#endif

#ifndef RSI_BLE_MTU_EXCHANGE_FROM_HOST
#define RSI_BLE_MTU_EXCHANGE_FROM_HOST 0
#endif
