/*******************************************************************************
* @file  rsi_ble.h
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

#ifndef RSI_BLE_H
#define RSI_BLE_H

#include <rsi_data_types.h>
#include <rsi_bt_common.h>
#include <rsi_ble_apis.h>

/******************************************************
 * *                      Macros
 * ******************************************************/

#define RSI_BLE_STATE_NONE       0x00
#define RSI_BLE_STATE_CONNECTION 0x01
#define RSI_BLE_STATE_DSICONNECT 0x02
#define RSI_BLE_MAX_REQ_LIST     0x05

#define SI_LE_BUFFER_AVL         0
#define SI_LE_BUFFER_FULL        1
#define SI_LE_BUFFER_IN_PROGRESS 2

#define BLE_VENDOR_RF_TYPE_CMD_OPCODE               0xFC14
#define BLE_VENDOR_WHITELIST_USING_ADV_DATA_PAYLOAD 0xFC1B

/******************************************************
 * *                    Constants
 * ******************************************************/

/******************************************************
 * *                   Enumerations
 * ******************************************************/

// enumeration for BLE command request codes
typedef enum rsi_ble_cmd_request_e {
  RSI_BLE_REQ_ADV                                    = 0x0075,
  RSI_BLE_REQ_SCAN                                   = 0x0076,
  RSI_BLE_REQ_CONN                                   = 0x0077,
  RSI_BLE_REQ_DISCONNECT                             = 0x0078,
  RSI_BLE_GET_DEV_STATE                              = 0x0079,
  RSI_BLE_CMD_CONN_PARAMS_UPDATE                     = 0x007A,
  RSI_BLE_REQ_START_ENCRYPTION                       = 0x007B,
  RSI_BLE_REQ_SMP_PAIR                               = 0x007C,
  RSI_BLE_SMP_PAIR_RESPONSE                          = 0x007D,
  RSI_BLE_SMP_PASSKEY                                = 0x007E,
  RSI_BLE_REQ_PROFILES                               = 0x007F,
  RSI_BLE_REQ_PROFILE                                = 0x0080,
  RSI_BLE_REQ_CHAR_SERVICES                          = 0x0081,
  RSI_BLE_REQ_INC_SERVICES                           = 0x0082,
  RSI_BLE_REQ_READ_BY_UUID                           = 0x0083,
  RSI_BLE_REQ_DESC                                   = 0x0084,
  RSI_BLE_REQ_READ_VAL                               = 0x0085,
  RSI_BLE_REQ_MULTIPLE_READ                          = 0x0086,
  RSI_BLE_REQ_LONG_READ                              = 0x0087,
  RSI_BLE_REQ_WRITE                                  = 0x0088,
  RSI_BLE_REQ_WRITE_NO_ACK                           = 0x0089,
  RSI_BLE_REQ_LONG_WRITE                             = 0x008A,
  RSI_BLE_REQ_PREPARE_WRITE                          = 0x008B,
  RSI_BLE_REQ_EXECUTE_WRITE                          = 0x008C,
  RSI_BLE_ADD_SERVICE                                = 0x0092,
  RSI_BLE_ADD_ATTRIBUTE                              = 0x0093,
  RSI_BLE_SET_LOCAL_ATT_VALUE                        = 0x0094,
  RSI_BLE_GET_LOCAL_ATT_VALUE                        = 0x0095,
  RSI_BLE_CMD_NOTIFY                                 = 0x0096,
  RSI_BLE_CMD_INDICATE                               = 0x0097,
  RSI_BLE_SET_ADVERTISE_DATA                         = 0x009C,
  RSI_BLE_GET_LE_PING                                = 0x00A1,
  RSI_BLE_SET_LE_PING                                = 0x00A2,
  RSI_BLE_SET_RANDOM_ADDRESS                         = 0x00A3,
  RSI_BLE_ENCRYPT                                    = 0x00A4,
  RSI_BLE_CMD_READ_RESP                              = 0x00A5,
  RSI_BLE_SET_SCAN_RESPONSE_DATA                     = 0x00A8,
  RSI_BLE_LE_WHITE_LIST                              = 0x00AA,
  RSI_BLE_CMD_REMOVE_SERVICE                         = 0x00AB,
  RSI_BLE_CMD_REMOVE_ATTRIBUTE                       = 0x00AC,
  RSI_BLE_PROCESS_RESOLV_LIST                        = 0x00AD,
  RSI_BLE_GET_RESOLVING_LIST_SIZE                    = 0x00AE,
  RSI_BLE_SET_ADDRESS_RESOLUTION_ENABLE              = 0x00AF,
  RSI_BLE_REQ_READ_PHY                               = 0x00B0,
  RSI_BLE_REQ_SET_PHY                                = 0x00B1,
  RSI_BLE_SET_DATA_LEN                               = 0x00B2,
  RSI_BLE_READ_MAX_DATA_LEN                          = 0X00B3,
  RSI_BLE_SET_PRIVACY_MODE                           = 0x00B4,
  RSI_BLE_CBFC_CONN_REQ                              = 0x00B5,
  RSI_BLE_CBFC_CONN_RESP                             = 0x00B6,
  RSI_BLE_CBFC_TX_DATA                               = 0x00B7,
  RSI_BLE_CBFC_DISCONN                               = 0x00B8,
  RSI_BLE_LE_LTK_REQ_REPLY                           = 0X00BA,
  RSI_BLE_RX_TEST_MODE                               = 0x00BB,
  RSI_BLE_TX_TEST_MODE                               = 0x00BC,
  RSI_BLE_END_TEST_MODE                              = 0x00BD,
  RSI_BLE_PER_TX_MODE                                = 0x00BF,
  RSI_BLE_PER_RX_MODE                                = 0x00C0,
  RSI_BLE_CMD_ATT_ERROR                              = 0x00C1,
  RSI_BLE_REQ_PROFILES_ASYNC                         = 0x00F2,
  RSI_BLE_REQ_PROFILE_ASYNC                          = 0x00F3,
  RSI_BLE_GET_CHARSERVICES_ASYNC                     = 0x00F4,
  RSI_BLE_GET_INCLUDESERVICES_ASYNC                  = 0x00F5,
  RSI_BLE_READCHARVALUEBYUUID_ASYNC                  = 0x00F6,
  RSI_BLE_GET_ATTRIBUTE_ASYNC                        = 0x00F7,
  RSI_BLE_GET_DESCRIPTORVALUE_ASYNC                  = 0x00F8,
  RSI_BLE_GET_MULTIPLEVALUES_ASYNC                   = 0x00F9,
  RSI_BLE_GET_LONGDESCVALUES_ASYNC                   = 0x00FA,
  RSI_BLE_SET_DESCVALUE_ASYNC                        = 0x00FB,
  RSI_BLE_SET_PREPAREWRITE_ASYNC                     = 0x00FC,
  RSI_BLE_EXECUTE_LONGDESCWRITE_ASYNC                = 0x00FD,
  RSI_BLE_SET_SMP_PAIRING_CAPABILITY_DATA            = 0x0101,
  RSI_BLE_CONN_PARAM_RESP_CMD                        = 0x0105,
  RSI_BLE_CMD_INDICATE_CONFIRMATION                  = 0x0106,
  RSI_BLE_MTU_EXCHANGE_REQUEST                       = 0x0107,
  RSI_BLE_CMD_SET_WWO_RESP_NOTIFY_BUF_INFO           = 0x0108,
  RSI_BLE_CMD_WRITE_RESP                             = 0x010A,
  RSI_BLE_CMD_PREPARE_WRITE_RESP                     = 0x010B,
  RSI_BLE_CMD_SET_LOCAL_IRK                          = 0x010C,
  RSI_BLE_CMD_SET_PROP_PROTOCOL_BLE_BANDEDGE_TXPOWER = 0x012A,
  RSI_BLE_CMD_MTU_EXCHANGE_RESP                      = 0x012B,
  RSI_BLE_CMD_SET_BLE_TX_POWER                       = 0x012D,
  RSI_BLE_CMD_INDICATE_SYNC                          = 0x016F,
#ifdef RSI_PROP_PROTOCOL_ENABLE
  RSI_PROP_PROTOCOL_CMD     = 0xE000,
  RSI_PROP_PROTOCOL_CMD_PER = 0xE001,
#endif
} rsi_ble_cmd_request_t;

// enumeration for BLE command response codes
typedef enum rsi_ble_cmd_resp_e {
  RSI_BLE_RSP_ADVERTISE                     = 0x0075,
  RSI_BLE_RSP_SCAN                          = 0x0076,
  RSI_BLE_RSP_CONNECT                       = 0x0077,
  RSI_BLE_RSP_DISCONNECT                    = 0x0078,
  RSI_BLE_RSP_QUERY_DEVICE_STATE            = 0x0079,
  RSI_BLE_RSP_CONN_PARAMS_UPDATE            = 0x007A,
  RSI_BLE_RSP_START_ENCRYPTION              = 0x007B,
  RSI_BLE_RSP_SMP_PAIR                      = 0x007C,
  RSI_BLE_RSP_SMP_PAIR_RESPONSE             = 0x007D,
  RSI_BLE_RSP_SMP_PASSKEY                   = 0x007E,
  RSI_BLE_RSP_PROFILES                      = 0x007F,
  RSI_BLE_RSP_PROFILE                       = 0x0080,
  RSI_BLE_RSP_CHAR_SERVICES                 = 0x0081,
  RSI_BLE_RSP_INC_SERVICES                  = 0x0082,
  RSI_BLE_RSP_READ_BY_UUID                  = 0x0083,
  RSI_BLE_RSP_DESC                          = 0x0084,
  RSI_BLE_RSP_READ_VAL                      = 0x0085,
  RSI_BLE_RSP_MULTIPLE_READ                 = 0x0086,
  RSI_BLE_RSP_LONG_READ                     = 0x0087,
  RSI_BLE_RSP_WRITE                         = 0x0088,
  RSI_BLE_RSP_WRITE_NO_ACK                  = 0x0089,
  RSI_BLE_RSP_LONG_WRITE                    = 0x008A,
  RSI_BLE_RSP_PREPARE_WRITE                 = 0x008B,
  RSI_BLE_RSP_EXECUTE_WRITE                 = 0x008C,
  RSI_BLE_RSP_INIT                          = 0x008D,
  RSI_BLE_RSP_DEINIT                        = 0x008E,
  RSI_BLE_RSP_SET_ANTENNA                   = 0x008F,
  RSI_BLE_RSP_ADD_SERVICE                   = 0x0092,
  RSI_BLE_RSP_ADD_ATTRIBUTE                 = 0x0093,
  RSI_BLE_RSP_SET_LOCAL_ATT_VALUE           = 0x0094,
  RSI_BLE_RSP_GET_LOCAL_ATT_VALUE           = 0x0095,
  RSI_BLE_RSP_NOTIFY                        = 0x0096,
  RSI_BLE_RSP_GET_LE_PING                   = 0x00A1,
  RSI_BLE_RSP_SET_LE_PING                   = 0x00A2,
  RSI_BLE_RSP_SET_RANDOM_ADDRESS            = 0x00A3,
  RSI_BLE_RSP_ENCRYPT                       = 0x00A4,
  RSI_BLE_RSP_READ_RESP                     = 0x00A5,
  RSI_BLE_RSP_LE_WHITE_LIST                 = 0x00AA,
  RSI_BLE_RSP_REMOVE_SERVICE                = 0x00AB,
  RSI_BLE_RSP_REMOVE_ATTRIBUTE              = 0x00AC,
  RSI_BLE_RSP_PROCESS_RESOLV_LIST           = 0x00AD,
  RSI_BLE_RSP_GET_RESOLVING_LIST_SIZE       = 0x00AE,
  RSI_BLE_RSP_SET_ADDRESS_RESOLUTION_ENABLE = 0x00AF,
  RSI_BLE_RSP_READ_PHY                      = 0x00B0,
  RSI_BLE_RSP_SET_PHY                       = 0x00B1,
  RSI_BLE_RSP_SET_DATA_LEN                  = 0x00B2,
  RSI_BLE_RSP_READ_MAX_DATA_LEN             = 0X00B3,
  RSI_BLE_RSP_PRIVACY_MODE                  = 0x00B4,
  RSI_BLE_RSP_CBFC_CONN_REQ                 = 0x00B5,
  RSI_BLE_RSP_CBFC_CONN_RESP                = 0x00B6,
  RSI_BLE_RSP_CBFC_TX_DATA                  = 0x00B7,
  RSI_BLE_RSP_CBFC_DISCONN                  = 0x00B8,

  RSI_BLE_RSP_LE_LTK_REQ_REPLY = 0X00BA,
  RSI_BLE_RSP_RX_TEST_MODE     = 0x00BB,
  RSI_BLE_RSP_TX_TEST_MODE     = 0x00BC,
  RSI_BLE_RSP_END_TEST_MODE    = 0x00BD,
  RSI_BLE_RSP_PER_TX_MODE      = 0x00BE,
  RSI_BLE_RSP_PER_RX_MODE      = 0x00BF,

  RSI_BLE_RSP_ATT_ERROR = 0x00C1,

  RSI_BLE_RSP_PROFILES_ASYNC              = 0x00F2,
  RSI_BLE_RSP_PROFILE_ASYNC               = 0x00F3,
  RSI_BLE_RSP_GET_CHARSERVICES_ASYNC      = 0x00F4,
  RSI_BLE_RSP_GET_INCLUDESERVICES_ASYNC   = 0x00F5,
  RSI_BLE_RSP_READCHARVALUEBYUUID_ASYNC   = 0x00F6,
  RSI_BLE_RSP_GET_ATTRIBUTE_ASYNC         = 0x00F7,
  RSI_BLE_RSP_GET_DESCRIPTORVALUE_ASYNC   = 0x00F8,
  RSI_BLE_RSP_GET_MULTIPLEVALUES_ASYNC    = 0x00F9,
  RSI_BLE_RSP_GET_LONGDESCVALUES_ASYNC    = 0x00FA,
  RSI_BLE_RSP_SET_DESCVALUE_ASYNC         = 0x00FB,
  RSI_BLE_RSP_SET_PREPAREWRITE_ASYNC      = 0x00FC,
  RSI_BLE_RSP_EXECUTE_LONGDESCWRITE_ASYNC = 0x00FD,

  RSI_BLE_RSP_SET_SMP_PAIRING_CAPABILITY_DATA        = 0x0101,
  RSI_BLE_RSP_CONN_PARAM_RESP                        = 0x0105,
  RSI_BLE_RSP_INDICATE_CONFIRMATION                  = 0x0106,
  RSI_BLE_RSP_MTU_EXCHANGE_REQUEST                   = 0x0107,
  RSI_BLE_RSP_SET_WWO_RESP_NOTIFY_BUF_INFO           = 0x0108,
  RSI_BLE_RSP_SET_LOCAL_IRK                          = 0x010C,
  RSI_BLE_RSP_SET_PROP_PROTOCOL_BLE_BANDEDGE_TXPOWER = 0x012A,
  RSI_BLE_RSP_MTU_EXCHANGE_RESP                      = 0x012B,
  RSI_BLE_RSP_SET_BLE_TX_POWER                       = 0x012D,
#ifdef RSI_PROP_PROTOCOL_ENABLE
  RSI_PROP_PROTOCOL_CMD_RSP     = 0xE000,
  RSI_PROP_PROTOCOL_CMD_RSP_PER = 0xE001,
#endif
} rsi_ble_cmd_resp_t;

// enumeration for BLE command response codes
typedef enum rsi_ble_event_e {
  RSI_BLE_EVENT_DISCONNECT                   = 0x1006,
  RSI_BLE_EVENT_GATT_ERROR_RESPONSE          = 0x1500,
  RSI_BLE_EVENT_GATT_DESC_VAL_RESPONSE       = 0x1501,
  RSI_BLE_EVENT_GATT_PRIMARY_SERVICE_BY_UUID = 0x1502,
  RSI_BLE_EVENT_GATT_READ_CHAR_SERVS       = 0x1503, //read by type: read char, include serivces and read value by uuid.
  RSI_BLE_EVENT_GATT_READ_INC_SERVS        = 0x1504, //read by type: read char, include serivces and read value by uuid.
  RSI_BLE_EVENT_GATT_READ_VAL_BY_UUID      = 0x1505, //read by type: read char, include serivces and read value by uuid.
  RSI_BLE_EVENT_GATT_READ_RESP             = 0x1506,
  RSI_BLE_EVENT_GATT_READ_BLOB_RESP        = 0x1507,
  RSI_BLE_EVENT_GATT_READ_MULTIPLE_RESP    = 0x1508,
  RSI_BLE_EVENT_GATT_PRIMARY_SERVICE_LIST  = 0x1509,
  RSI_BLE_EVENT_GATT_WRITE_RESP            = 0x150A,
  RSI_BLE_EVENT_GATT_PREPARE_WRITE_RESP    = 0x150B,
  RSI_BLE_EVENT_GATT_EXECUTE_WRITE_RESP    = 0x150C,
  RSI_BLE_EVENT_GATT_INDICATE_CONFIRMATION = 0x150D,
  RSI_BLE_EVENT_ADV_REPORT                 = 0x150E,
  RSI_BLE_EVENT_CONN_STATUS                = 0x150F,
  RSI_BLE_EVENT_SMP_REQUEST                = 0x1510,
  RSI_BLE_EVENT_SMP_RESPONSE               = 0x1511,
  RSI_BLE_EVENT_SMP_PASSKEY                = 0x1512,
  RSI_BLE_EVENT_SMP_FAILED                 = 0x1513,
  RSI_BLE_EVENT_GATT_NOTIFICATION          = 0x1514,
  RSI_BLE_EVENT_GATT_INDICATION            = 0x1515,
  RSI_BLE_EVENT_ENCRYPT_STARTED            = 0x1516,
  RSI_BLE_EVENT_GATT_WRITE                 = 0x1517,
  RSI_BLE_EVENT_LE_PING_TIME_EXPIRED       = 0x1518,
  RSI_BLE_EVENT_PREPARE_WRITE              = 0x1519,
  RSI_BLE_EVENT_EXECUTE_WRITE              = 0x151A,
  RSI_BLE_EVENT_READ_REQ                   = 0x151B,
  RSI_BLE_EVENT_MTU                        = 0x151C,
  RSI_BLE_EVENT_SMP_PASSKEY_DISPLAY_EVENT  = 0x151D,
  RSI_BLE_EVENT_PHY_UPDATE_COMPLETE        = 0x151E,
  RSI_BLE_EVENT_DATA_LENGTH_UPDATE_COMPLETE = 0x151F,
  RSI_BLE_EVENT_SC_PASSKEY                  = 0x1520,
  RSI_BLE_EVENT_ENHANCE_CONN_STATUS         = 0x1521,
  RSI_BLE_EVENT_DIRECTED_ADV_REPORT         = 0x1522,
  RSI_BLE_EVENT_SECURITY_KEYS               = 0x1523,
  RSI_BLE_EVENT_PSM_CONN_REQ                = 0x1524,
  RSI_BLE_EVENT_PSM_CONN_COMPLETE           = 0x1525,
  RSI_BLE_EVENT_PSM_RX_DATA                 = 0x1526,
  RSI_BLE_EVENT_PSM_DISCONNECT              = 0x1527,
  RSI_BLE_EVENT_LE_LTK_REQUEST              = 0x152A,
  RSI_BLE_EVENT_CONN_UPDATE_COMPLETE        = 0x152B,
  RSI_BLE_EVENT_REMOTE_FEATURES             = 0x152C,
  RSI_BLE_EVENT_LE_MORE_DATA_REQ            = 0x152D,
  RSI_BLE_EVENT_REMOTE_CONN_PARAMS_REQUEST  = 0x153C,
  RSI_BLE_EVENT_CLI_SMP_RESPONSE            = 0x153D,
  RSI_BLE_EVENT_CHIP_MEMORY_STATS           = 0x1530,
  RSI_BLE_EVENT_SC_METHOD                   = 0x1540,
  RSI_BLE_EVENT_MTU_EXCHANGE_INFORMATION    = 0x1541,
} rsi_ble_event_t;

/********************************************************
 * *                 Structure Definitions
 * ******************************************************/

// GAP command structures
typedef struct rsi_ble_req_rand_s {
  //uint8, random address of the device to be set
  uint8_t rand_addr[RSI_DEV_ADDR_LEN];
} rsi_ble_req_rand_t;

// Advertising command structure
typedef struct rsi_ble_req_adv_s {
  /** Advertising Status \n 
      0 - disable \n 1 - enable*/
  uint8_t status;
  /** Advertising type used during advertising \n
      1. Advertising will be visible(discoverable) to all the devices. Scanning/Connection is also accepted from all devices. \n
      #define UNDIR_CONN     0x80 \n
      2. Advertising will be visible(discoverable) to the particular device mentioned in RSI_BLE_ADV_DIR_ADDR only. \n
      Scanning and Connection will be accepted from that device only. \n
      #define DIR_CONN           0x81 \n
      3. Advertising will be visible(discoverable) to all the devices. Scanning will be accepted from all the devices. \n
      Connection will be not be accepted from any device. \n
      #define UNDIR_SCAN         0x82 \n
      4. Advertising will be visible(discoverable) to all the devices. Scanning and Connection will not be accepted from any device. \n
      #define UNDIR_NON_CONN       0x83 \n
      5. Advertising will be visible(discoverable) to the particular device mentioned in RSI_BLE_ADV_DIR_ADDR only. \n
      Scanning and Connection will be accepted from that device only. \n
      #define DIR_CONN_LOW_DUTY_CYCLE      0x84  */
  uint8_t adv_type;
  /** Advertising filter type \n
      #define ALLOW_SCAN_REQ_ANY_CONN_REQ_ANY                        0x00 \n
      #define ALLOW_SCAN_REQ_WHITE_LIST_CONN_REQ_ANY                 0x01 \n
      #define ALLOW_SCAN_REQ_ANY_CONN_REQ_WHITE_LIST                 0x02 \n
      #define ALLOW_SCAN_REQ_WHITE_LIST_CONN_REQ_WHITE_LIST          0x03 */
  uint8_t filter_type;
  /** Address type of the device to which directed advertising has to be done \n
      #define LE_PUBLIC_ADDRESS                                     0x00 \n
      #define LE_RANDOM_ADDRESS                                     0x01 \n
      #define LE_RESOLVABLE_PUBLIC_ADDRESS                          0x02 \n
      #define LE_RESOLVABLE_RANDOM_ADDRESS                          0x03 */
  uint8_t direct_addr_type;
  /** Address of the device to which directed advertising has to be done */
  uint8_t direct_addr[RSI_DEV_ADDR_LEN];

  /** Advertising interval min 0x0020 to 0x4000 */
  uint16_t adv_int_min;

  /** Advertising interval max 0x0020 to 0x4000 */
  uint16_t adv_int_max;

  /** Address of the local device. \n
      #define LE_PUBLIC_ADDRESS                                     0x00 \n
      #define LE_RANDOM_ADDRESS                                     0x01 \n
      #define LE_RESOLVABLE_PUBLIC_ADDRESS                          0x02 \n
      #define LE_RESOLVABLE_RANDOM_ADDRESS                          0x03 */
  uint8_t own_addr_type;

  /** Advertising channel map. \n
      #define  RSI_BLE_ADV_CHANNEL_MAP    0x01 or 0x03 or 0x07 */
  uint8_t adv_channel_map;
} rsi_ble_req_adv_t;

// Advertising data command structure
typedef struct rsi_ble_req_adv_data_s {
  //uint8, advertising data length
  uint8_t data_len;
  //uint8, advertising data
  uint8_t adv_data[31];
} rsi_ble_req_adv_data_t;

// Adv data payload for whitelisting based on the payload
typedef struct rsi_ble_req_whitelist_using_payload_s {
  uint8_t opcode[2];
  uint8_t enable;
  uint8_t total_len;
  uint8_t data_compare_index;
  uint8_t len_for_compare_data;
  uint8_t adv_data_payload[31];
} rsi_ble_req_whitelist_using_payload_t;

#define BLE_PROTOCOL  0x01
#define PROP_PROTOCOL 0x02
// Set BLE PROTOCOL and PROP_PROTOCOL bandedge tx power cmd_ix=0x012A
typedef struct rsi_ble_set_prop_protocol_ble_bandedge_tx_power_s {
  uint8_t protocol;
  int8_t tx_power;
} rsi_ble_set_prop_protocol_ble_bandedge_tx_power_t;

#define ADV_ROLE              0x01
#define SCAN_AND_CENTRAL_ROLE 0x02
#define PERIPHERAL_ROLE       0x03
#define CONN_ROLE             0x04
// Set BLE tx power per role cmd_ix=0x012D
typedef struct rsi_ble_set_ble_tx_power_s {
  uint8_t role;
  //Address of the device
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
  uint8_t tx_power;
} rsi_ble_set_ble_tx_power_t;

//Scan response data command structure
typedef struct rsi_ble_req_scanrsp_data_s {
  //uint8, scanresponse data length
  uint8_t data_len;
  //uint8, scanresponse data
  uint8_t scanrsp_data[31];
} rsi_ble_req_scanrsp_data_t;

//Scan command structure
typedef struct rsi_ble_req_scan_s {

  /** Scanning Status \n 
      0 - disable \n 1 - enable*/
  uint8_t status;

  /** Scanning type \n
  #define SCAN_TYPE_ACTIVE                              0x01 \n
  #define SCAN_TYPE_PASSIVE                             0x00 */
  uint8_t scan_type;

  /** To filter incoming advertising reports \n
   FILTERING_DISABLED = 0 (default) \n
   WHITELIST_FILTERING = 1
   @note     In order to allow only whitelisted devices, need to add bd_addr
   into whitelist by calling @ref rsi_ble_addto_whitelist() API  */
  uint8_t filter_type;

  /** Address type of the local device \n
      #define LE_PUBLIC_ADDRESS                  0x00 \n
      #define LE_RANDOM_ADDRESS                  0x01 \n
      #define LE_RESOLVABLE_PUBLIC_ADDRESS       0x02 \n
      #define LE_RESOLVABLE_RANDOM_ADDRESS       0x03  */
  uint8_t own_addr_type;

  /** Scan interval \n
     This is defined as the time interval from when the Controller started
	 its last LE scan until it begins the subsequent LE scan. \n
     Range: 0x0004 to 0x4000 */
  uint16_t scan_int;

  /** Scan window \n
      The duration of the LE scan. LE_Scan_Window shall be less than or equal to LE_Scan_Interval \n
      Range: 0x0004 to 0x4000 */
  uint16_t scan_win;

} rsi_ble_req_scan_t;

//ENCRYPT COMMAND STRUCTURE

typedef struct rsi_ble_encrypt_s {

  uint8_t key[16];
  uint8_t data[16];

} rsi_ble_encrypt_t;

//White list structure
typedef struct rsi_ble_white_list_s {
  //This bit is used to add or delet the address form/to whit list
  uint8_t addordeltowhitlist;
  //Address of the device
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
  //Address type
  uint8_t bdaddressType;

} rsi_ble_white_list_t;

//Connect command structure
typedef struct rsi_ble_req_conn_s {
  //uint8, address type of the device to connect
  uint8_t dev_addr_type;
  //uint8[6], address of the device to connect
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
  //uint16, scan interval
  uint16_t le_scan_interval;
  //uint16, scan window
  uint16_t le_scan_window;
  //uint16, minimum connection interval
  uint16_t conn_interval_min;
  //uint16, maximum connection interval
  uint16_t conn_interval_max;
  //uint16, connection latency
  uint16_t conn_latency;
  //uint16, supervision timeout
  uint16_t supervision_tout;
} rsi_ble_req_conn_t;

//Disconnect command structure
typedef struct rsi_ble_req_disconnect_s {
  //uint8[6], address of the device to disconnect
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
} rsi_ble_req_disconnect_t;

//SMP protocol structures

//start encryption cmd structures
typedef struct rsi_ble_start_encryption_s {
  //uint8[6], address of the connected device
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
  //uint16, remote device ediv value
  uint16_t ediv;
  //uint8[8], remote device rand value
  uint8_t rand[8];
  //uint8[16], remote device ltk value
  uint8_t ltk[16];
} rsi_ble_strat_encryption_t;

//SMP Pair Request command structure = 0x007C
typedef struct rsi_ble_req_smp_pair_s {
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
  uint8_t io_capability;
  uint8_t mitm_req;
} rsi_ble_req_smp_pair_t;

//SMP Response command structure = 0x007D
typedef struct rsi_ble_smp_response_s {
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
  uint8_t io_capability;
  uint8_t mitm_req;
} rsi_ble_smp_response_t;

//SMP Passkey command structure, cmd_ix - 0x007E
typedef struct rsi_ble_smp_passkey_s {
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
  uint8_t reserved[2];
  uint32_t passkey;
} rsi_ble_smp_passkey_t;

//LE ping get auth payload timeout command structure, cmd_ix - 0x00A1
typedef struct rsi_ble_get_le_ping_timeout_s {
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
} rsi_ble_get_le_ping_timeout_t;

//LE ping get auth payload timeout command structure, cmd_ix - 0x00A2
typedef struct rsi_ble_rsp_get_le_ping_timeout_s {
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
  uint16_t time_out;
} rsi_ble_rsp_get_le_ping_timeout_t;

//LE ping get auth payload timeout command structure, cmd_ix - 0x00A2
typedef struct rsi_ble_set_le_ping_timeout_s {
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
  uint16_t time_out;
} rsi_ble_set_le_ping_timeout_t;

//LE Add Device to resolvlist command structure, cmd_ix - 0x00AD
typedef struct rsi_ble_resolvlist_s {
  uint8_t process_type;
  uint8_t remote_dev_addr_type;
  uint8_t remote_dev_addr[RSI_DEV_ADDR_LEN];
  uint8_t peer_irk[16];
  uint8_t local_irk[16];
} rsi_ble_resolvlist_t;

//LE Get resolvlist size command structure, cmd_ix - 0x00AE
typedef struct rsi_ble_get_resolving_list_size_s {

  uint8_t size;
} rsi_ble_get_resolving_list_size_t;

//LE Set address resolution enable command structure, cmd_ix - 0x00AF
typedef struct rsi_ble_set_addr_resolution_enable_s {

  uint8_t enable;
  uint8_t reserved;
  uint16_t tout;
} rsi_ble_set_addr_resolution_enable_t;

//LE conn params update command structure, cmd_ix - 0x007A
typedef struct rsi_ble_cmd_conn_params_update_s {
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
  uint16_t min_interval;
  uint16_t max_interval;
  uint16_t latency;
  uint16_t timeout;
} rsi_ble_cmd_conn_params_update_t;

//LE read phy request command structure, cmd_ix - 0x00B0
typedef struct rsi_ble_req_read_phy_s {
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
} rsi_ble_req_read_phy_t;

//LE set phy command reasponse structure, cmd_ix - 0x00B1
typedef struct rsi_ble_set_phy_s {
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
  uint8_t all_phy;
  uint8_t tx_phy;
  uint8_t rx_phy;
  uint8_t reserved;
  uint16_t phy_options;
} rsi_ble_set_phy_t;

//LE set data length command reasponse structure, cmd_ix - 0x00B2
typedef struct rsi_ble_setdatalength_s {
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
  uint16_t txoctets;
  uint16_t txtime;
} rsi_ble_setdatalength_t;

//LE set privacy mode command structure, cmd_ix - 0x00B4
typedef struct rsi_ble_set_privacy_mode_s {
  uint8_t remote_dev_addr_type;
  uint8_t remote_dev_addr[RSI_DEV_ADDR_LEN];
  uint8_t privacy_mode;
} rsi_ble_set_privacy_mode_t;

//LE cbfc connection req command structure, cmd_ix - 0x00B5
typedef struct rsi_ble_cbfc_conn_req_s {
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
  uint8_t psm;
} rsi_ble_cbfc_conn_req_t;

//LE cbfc connection resp command structure, cmd_ix - 0x00B6
typedef struct rsi_ble_cbfc_conn_resp_s {
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
  uint16_t lcid;
  uint8_t result;
} rsi_ble_cbfc_conn_resp_t;

//LE cbfc data TX command structure, cmd_ix - 0x00B7
typedef struct rsi_ble_cbfc_data_tx_s {
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
  uint16_t lcid;
  uint16_t len;
  uint8_t data[RSI_DEV_ATT_LEN];
} rsi_ble_cbfc_data_tx_t;

//LE cbfc disconn command structure, cmd_ix - 0x00B8
typedef struct rsi_ble_cbfc_disconn_s {
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
  uint16_t lcid;
} rsi_ble_cbfc_disconn_t;

//LE RX text mode (cmd), cmd_ix = 0x00BB
typedef struct rsi_ble_rx_test_mode_s {
  uint8_t rx_channel;
  uint8_t phy;
  uint8_t modulation;
} rsi_ble_rx_test_mode_t;

//LE TX test mode (cmd), cmd_ix = 0x00BC
typedef struct rsi_ble_tx_test_mode_s {
  uint8_t tx_channel;
  uint8_t phy;
  uint8_t tx_len;
  uint8_t tx_data_mode;
} rsi_ble_tx_test_mode_t;

//LE End test mode (cmd), cmd_ix = 0x00BD
typedef struct rsi_ble_end_test_mode_s {
  uint16_t num_of_pkts;
} rsi_ble_end_test_mode_t;

typedef struct rsi_ble_set_le_ltkreqreply_s {
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
  uint8_t replytype;
  uint8_t localltk[16];
} rsi_ble_set_le_ltkreqreply_t;

// GATT structures

// GATT Profiles list request structure
typedef struct rsi_ble_req_profiles_list_s {
  //uint8[6], remote device address.
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
  //uint8[2], profiles search will be start from start_handle.
  uint16_t start_handle;
  //uint8[2], profile search will stop with end_handle.
  uint16_t end_handle;
} rsi_ble_req_profiles_list_t;

// GATT Profile request structure
typedef struct rsi_ble_req_profile_s {
  //uint8[6], remote device address.
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
  //uint8[2], reserved.
  uint8_t reserved[2];
  //uint8[20], profile UUID.
  uuid_t profile_uuid;
} rsi_ble_req_profile_t;

// GATT Profile response  structure
//profile_descriptors_t;

// GATT multiple characteristic services request structure
typedef struct rsi_ble_req_char_services_s {
  //uint8[6], remote device address.
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
  //uint8[2], search will be start from start_handle.
  uint8_t start_handle[2];
  //uint8[2], search will be end with end_handle.
  uint8_t end_handle[2];
} rsi_ble_req_char_services_t;

// GATT include service query request structure
typedef struct rsi_ble_req_inc_services_s {
  //uint8[6], remote device address.
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
  //uint8[2], search will be start from start_handle.
  uint8_t start_handle[2];
  //uint8[2], search will be end with end_handle.
  uint8_t end_handle[2];
} rsi_ble_req_inc_services_t;

// GATT read value by UUID request structure
typedef struct rsi_ble_req_char_val_by_uuid_s {
  //uint8[6], remote device address.
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
  //uint8[2], search will be start from start_handle.
  uint8_t start_handle[2];
  //uint8[2], search will be end with end_handle.
  uint8_t end_handle[2];
  //uint8[2], reserved.
  uint8_t reserved[2];
  //uint8[20], search UUID value.
  uuid_t char_uuid;
} rsi_ble_req_char_val_by_uuid_t;

// GATT read value by UUID response structure
//rsi_ble_resp_att_value_t

// GATT multiple attribute descriptors request structure
typedef struct rsi_ble_req_att_descs_s {
  //uint8[6], remote device address.
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
  //uint8[2], search will be start from start_handle.
  uint8_t start_handle[2];
  //uint8[2], search will be end with end_handle.
  uint8_t end_handle[2];
} rsi_ble_req_att_descs_t;

// GATT attribute value request structure
typedef struct rsi_ble_req_att_value_s {
  //uint8[6], remote device address.
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
  //uint8[2], attribute handle.
  uint8_t handle[2];
} rsi_ble_req_att_value_t;

// GATT multiple attribute values request structure
typedef struct rsi_ble_req_multiple_att_val_s {
  //uint8[6], remote device address.
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
  //uint8, number of attributes handles.
  uint8_t num_of_handles;
  //uint8, reserved
  uint8_t reserved;
  //(uint16 * 5), list of attributes handles.
  uint16_t handles[RSI_BLE_MAX_RESP_LIST];

} rsi_ble_req_multi_att_values_t;

// GATT multiple attribute values response structure
//rsi_ble_resp_att_value_t

// GATT long attribute value request structure
typedef struct rsi_ble_req_long_att_value_s {
  //uint8[6], remote device address.
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
  //uint8[2], attribute handle.
  uint16_t handle;
  //uint16, attribute value offset .
  uint16_t offset;
} rsi_ble_req_long_att_value_t;

// GATT long attribute value response structure
//rsi_ble_resp_att_value_t

// GATT write attribute value request structure
typedef struct rsi_ble_set_att_val_s {
  //uint8[6], remote device address.
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
  //uint8[2], attribute handle.
  uint8_t handle[2];
  //uint8, length of attribute value.
  uint8_t length;
  //uint8[25], attribute value.
  uint8_t att_value[RSI_DEV_ATT_LEN];
} rsi_ble_set_att_value_t;

// GATT write attribute value without ack request structure
typedef struct rsi_ble_set_att_cmd_s {
  //uint8[6], remote device address.
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
  //uint8[2], attribute handle.
  uint8_t handle[2];
  //uint8, length of attribute value.
  uint8_t length;
  //uint8[25], attribute value.
  uint8_t att_value[RSI_DEV_ATT_LEN];
} rsi_ble_set_att_cmd_t;

// GATT write long attribute value request structure
typedef struct rsi_ble_set_long_att_val_s {
  //uint8[6], remote device address.
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
  //uint8[2], attribute handle.
  uint8_t handle[2];
  //uint8[2], attribute value offset.
  uint8_t offset[2];
  //uint8, length of attribute value.
  uint8_t length;
  //uint8[40], attribute value.
  uint8_t att_value[RSI_DEV_ATT_LEN];
} rsi_ble_set_long_att_value_t;

// GATT prepare write value request structure
typedef struct rsi_ble_req_prepare_write_s {
  //uint8[6], remote device address.
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
  //uint8[2], attribute handle.
  uint8_t handle[2];
  //uint8[2], attribute value offset.
  uint8_t offset[2];
  //uint8, length of attribute value.
  uint8_t length;
  //uint8[40], attribute value.
  uint8_t att_value[RSI_DEV_ATT_LEN];
} rsi_ble_req_prepare_write_t;

// GATT execute write request structure
typedef struct rsi_ble_req_execute_write_s {
  //uint8[6], remote device address.
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
  //uint8, execute flag(write/don't write).
  uint8_t flag;
} rsi_ble_req_execute_write_t;

//RSI_BLE_CONN_PARAM_RESP_CMD, cmd_id: 0x0105
typedef struct rsi_ble_cmd_conn_param_resp {
  //uint8[6], remote device address.
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
  //uint8, accept or reject the remote conn param request
  uint8_t status;
} rsi_ble_cmd_conn_param_resp_t;

//GATT Events

//GATT Service
// add new GATT service request structure
typedef struct rsi_ble_req_add_serv_s {
  //uint8[20], service_uuid.
  uuid_t service_uuid;
  //uint8, number of attribute in the service need to use.
  uint8_t num_of_attributes;
  //uint8, total attributes value(data) size.
  uint8_t total_att_datasize;
} rsi_ble_req_add_serv_t;

// write or change local attribute value request structure
typedef struct rsi_ble_set_local_att_value_s {
  //uint8[2], attribute handle.
  uint16_t handle;
  //uint8[2], attribute value length.
  uint16_t data_len;
  //uint8[31], attribute value (data).
  uint8_t data[RSI_DEV_ATT_LEN];
} rsi_ble_set_local_att_value_t;

// write or change local attribute value request structure
typedef struct rsi_ble_notify_att_value_s {
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
  //uint8[2], attribute handle.
  uint16_t handle;
  //uint8[2], attribute value length.
  uint16_t data_len;
  //uint8[31], attribute value (data).
  uint8_t data[RSI_DEV_ATT_LEN];
} rsi_ble_notify_att_value_t;

// set wo_resp and notify buffer info
typedef struct rsi_ble_set_wo_resp_notify_buf_info_s {
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
  // uint8_t  buffer config mode 0. small buf mode, 1. big buf mode
  uint8_t buf_mode;
  // uint8_t  buffer count
  uint8_t buf_count;

} rsi_ble_set_wo_resp_notify_buf_info_t;

// indicate confirmation structure
typedef struct rsi_ble_indicate_confirm_s {
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
} rsi_ble_indicate_confirm_t;

// read local attribute value request structure
typedef struct rsi_ble_get_local_att_value_s {
  //uint8[2], attribute handle.
  uint16_t handle;
} rsi_ble_get_local_att_value_t;

typedef struct rsi_ble_gatt_read_response_s {
  //uint8[6], remote device address.
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
  //uint8[2], attribute handle.
  //	uint16_t      handle;
  //	uint16_t      offset;
  uint8_t type;
  uint8_t reserved;
  //uint8[2], attribute value length.
  uint16_t data_len;
  //uint8[31], attribute value (data).
  uint8_t data[RSI_DEV_ATT_LEN];
} rsi_ble_gatt_read_response_t;

// Att write/ execute write respomse cmd = 0x010A
typedef struct rsi_ble_gatt_write_response_s {
  //uint8[6], remote device address.
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
  // uint8_t   response type
  uint8_t type;
} rsi_ble_gatt_write_response_t;

// Att prepare write response cmd = 0x010B
typedef struct rsi_ble_gatt_prepare_write_response_s {
  //uint8[6], remote device address.
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
  uint16_t handle;
  uint16_t offset;
  uint16_t data_len;
  uint8_t data[RSI_DEV_ATT_LEN];
} rsi_ble_gatt_prepare_write_response_t;

// BLE Set local irk cmd id = 0x010C
typedef struct rsi_ble_set_local_irk_s {
  // uint8[16], local device irk
  uint8_t irk[16];
} rsi_ble_set_local_irk_t;

//attribute codes
#define RSI_BLE_ATT_EXCHANGE_MTU_REQUEST       0x02
#define RSI_BLE_ATT_FIND_INFORMATION_REQUEST   0x04
#define RSI_BLE_ATT_FIND_BY_TYPE_VALUE_REQUEST 0x06
#define RSI_BLE_ATT_READ_BY_TYPE_REQUEST       0x08
#define RSI_BLE_ATT_READ_REQUEST               0x0A
#define RSI_BLE_ATT_READ_BLOB_REQUEST          0x0C
#define RSI_BLE_ATT_READ_MULTIPLE_REQUEST      0x0E
#define RSI_BLE_ATT_READ_BY_GROUP_TYPE_REQUEST 0x10
#define RSI_BLE_ATT_WRITE_REQUEST              0x12
#define RSI_BLE_ATT_PREPARE_WRITE_REQUEST      0x16
#define RSI_BLE_ATT_EXECUTE_WRITE_REQUEST      0x18

// Att error response cmd = 0x00C1
typedef struct rsi_ble_att_error_response_s {
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
  uint8_t req_opcode;
  uint16_t att_handle;
  uint8_t err_code;
} rsi_ble_att_error_response_t;

//white list(cmd), cmd_ix = 0x00AB
typedef struct rsi_ble_gatt_remove_serv_s {
  uint32_t serv_hndler;
} rsi_ble_gatt_remove_serv_t;

//white list(cmd), cmd_ix = 0x00AC
typedef struct rsi_ble_gatt_remove_att_s {
  uint32_t serv_hndler;
  uint16_t att_hndl;
} rsi_ble_gatt_remove_att_t;

// rf type command structure
typedef struct rsi_ble_vendor_rf_type_s {
  uint8_t opcode[2];
  uint8_t ble_power_index;
} rsi_ble_vendor_rf_type_t;

// rf type command structure
typedef struct rsi_ble_mtu_exchange_s {
  uint8_t dev_addr[6];
  uint8_t req_mtu_size;
} rsi_ble_mtu_exchange_t;

// mtu exchange resp command structure
typedef struct rsi_ble_mtu_exchange_resp_s {
  uint8_t dev_addr[6];
  uint8_t req_mtu_size;
} rsi_ble_mtu_exchange_resp_t;

// Driver BLE control block
struct rsi_ble_cb_s {
  // GAP Callbacks
  rsi_ble_on_adv_report_event_t ble_on_adv_report_event;
  rsi_ble_on_connect_t ble_on_conn_status_event;
  rsi_ble_on_disconnect_t ble_on_disconnect_event;
  rsi_ble_on_le_ping_payload_timeout_t ble_on_le_ping_time_expired_event;
  rsi_ble_on_conn_update_complete_t ble_on_conn_update_complete_event;
  rsi_ble_on_remote_features_t ble_on_remote_features_event;
  rsi_ble_on_le_more_data_req_t ble_on_le_more_data_req_event;
  rsi_ble_on_remote_conn_params_request_t ble_on_remote_conn_params_request_event;

  //SMP Callbackes
  rsi_ble_on_smp_request_t ble_on_smp_request_event;
  rsi_ble_on_smp_response_t ble_on_smp_response_event;
  rsi_ble_on_smp_response_t ble_on_cli_smp_response_event;
  rsi_ble_on_smp_passkey_t ble_on_smp_passkey_event;
  rsi_ble_on_smp_failed_t ble_on_smp_fail_event;
  rsi_ble_on_encrypt_started_t ble_on_smp_encrypt_started;
  rsi_ble_on_sc_method_t ble_on_sc_method_event;

  // GATT Callbacks
  rsi_ble_on_profiles_list_resp_t ble_on_profiles_list_resp;
  rsi_ble_on_profile_resp_t ble_on_profile_resp;
  rsi_ble_on_char_services_resp_t ble_on_char_services_resp;
  rsi_ble_on_inc_services_resp_t ble_on_inc_services_resp;
  rsi_ble_on_att_desc_resp_t ble_on_att_desc_resp;
  rsi_ble_on_read_resp_t ble_on_read_resp;
  rsi_ble_on_write_resp_t ble_on_write_resp;
  rsi_ble_on_gatt_write_event_t ble_on_gatt_events;
  rsi_ble_on_gatt_prepare_write_event_t ble_on_prepare_write_event;
  rsi_ble_on_execute_write_event_t ble_on_execute_write_event;
  rsi_ble_on_read_req_event_t ble_on_read_req_event;
  rsi_ble_on_mtu_event_t ble_on_mtu_event;
  rsi_ble_on_gatt_error_resp_t ble_on_gatt_error_resp_event;
  rsi_ble_on_gatt_desc_val_event_t ble_on_gatt_desc_val_resp_event;
  rsi_ble_on_event_profiles_list_t ble_on_profiles_list_event;
  rsi_ble_on_event_profile_by_uuid_t ble_on_profile_by_uuid_event;
  rsi_ble_on_event_read_by_char_services_t ble_on_read_by_char_services_event;
  rsi_ble_on_event_read_by_inc_services_t ble_on_read_by_inc_services_event;
  rsi_ble_on_event_read_att_value_t ble_on_read_att_value_event;
  rsi_ble_on_event_read_resp_t ble_on_read_resp_event;
  rsi_ble_on_event_write_resp_t ble_on_write_resp_event;
  rsi_ble_on_event_indicate_confirmation_t ble_on_indicate_confirmation_event;
  rsi_ble_on_event_prepare_write_resp_t ble_on_prepare_write_resp_event;

  rsi_ble_on_mtu_exchange_info_t ble_on_mtu_exchange_info_event;

  rsi_ble_on_phy_update_complete_t ble_on_phy_update_complete_event;
  rsi_ble_on_data_length_update_t rsi_ble_on_data_length_update_event;

  rsi_ble_on_enhance_connect_t ble_on_enhance_conn_status_event;
  rsi_ble_on_directed_adv_report_event_t ble_on_directed_adv_report_event;
  rsi_ble_on_le_ltk_req_event_t ble_on_le_ltk_req_event;
  rsi_ble_on_smp_passkey_display_t ble_on_smp_passkey_display;
  rsi_ble_on_sc_passkey_t ble_on_sc_passkey;
  rsi_ble_on_le_security_keys_t ble_on_le_security_keys_event;

  //L2CAP CBFC callbacks
  rsi_ble_on_cbfc_conn_req_event_t ble_on_cbfc_conn_req_event;
  rsi_ble_on_cbfc_conn_complete_event_t ble_on_cbfc_conn_complete_event;
  rsi_ble_on_cbfc_rx_data_event_t ble_on_cbfc_rx_data_event;
  rsi_ble_on_cbfc_disconn_event_t ble_on_cbfc_disconn_event;
  chip_ble_buffers_stats_handler_t ble_on_chip_memory_status_event;
};

/******************************************************
 * * BLE internal function declarations
 * ******************************************************/

void rsi_ble_callbacks_handler(rsi_bt_cb_t *ble_cb, uint16_t rsp_type, uint8_t *payload, uint16_t payload_length);
int32_t rsi_ble_driver_send_cmd(uint16_t cmd, void *cmd_struct, void *resp);
int32_t rsi_ble_white_list_using_adv_data(uint8_t enable,
                                          uint8_t data_compare_index,
                                          uint8_t len_for_compare_data,
                                          uint8_t *payload);
int32_t rsi_ble_get_multiple_att_values(uint8_t *dev_addr,
                                        uint8_t num_of_handlers,
                                        uint16_t *handles,
                                        rsi_ble_resp_att_value_t *p_att_vals);
#endif
