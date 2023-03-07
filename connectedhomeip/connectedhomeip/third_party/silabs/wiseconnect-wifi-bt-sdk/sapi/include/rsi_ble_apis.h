/*******************************************************************************
* @file  rsi_ble_apis.h
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

#ifndef RSI_BLE_APIS_H
#define RSI_BLE_APIS_H

#include <rsi_data_types.h>
#include <rsi_utils.h>
#include <rsi_bt_common.h>

/******************************************************
 * *                      Macros
 * ******************************************************/

#define RSI_BLE_MAX_RESP_LIST   0x05
#define RSI_MAX_ADV_REPORT_SIZE 31

#ifndef BLE_OUTPUT_POWER_FRONT_END_LOSS
#define BLE_OUTPUT_POWER_FRONT_END_LOSS 0 /* db */
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
******************************************************/

// GAP Event structures

//Advertise report event structure
typedef struct rsi_ble_event_adv_report_s {
  /**Address type of the advertising device */
  uint8_t dev_addr_type;
  /**Address of the advertising device*/
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
  /**Raw advertisement data length*/
  uint8_t adv_data_len;
  /**advertisement data*/
  uint8_t adv_data[RSI_MAX_ADV_REPORT_SIZE];
  /**Signal strength*/
  int8_t rssi;
  /** Report type \n
       0x00	Connectable and scannable undirected advertising (ADV_IND) \n
       0x01	Connectable directed advertising (ADV_DIRECT_IND) \n
       0x02	Scannable undirected advertising (ADV_SCAN_IND)  \n
       0x03	Non connectable undirected advertising (ADV_NONCONN_IND) \n
       0x04	Scan Response (SCAN_RSP) \n
       All other values	Reserved for future use*/
  uint8_t report_type;
} rsi_ble_event_adv_report_t;

//Connection status event structure
typedef struct rsi_ble_event_conn_status_s {
  /**Address type of the connected device*/
  uint8_t dev_addr_type;
  /**Address of the connected device*/
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
  /**status of the connection - success/failure*/
  uint8_t status;

} rsi_ble_event_conn_status_t;

//enhance connection status event structure
typedef struct rsi_ble_event_enhnace_conn_status_s {
  /**Device address type of the Connected Remote Device*/
  uint8_t dev_addr_type;
  /**Device address of the remote device.*/
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
  /**Local Device resolvable address*/
  uint8_t local_resolvlable_addr[RSI_DEV_ADDR_LEN];
  /**Remote Device resolvable address*/
  uint8_t peer_resolvlable_addr[RSI_DEV_ADDR_LEN];
  /**The role of the device - master/ slave*/
  uint8_t role;
  /**Connection interval used on this connection. Range: 0x0006 to 0x0C80*/
  uint16_t conn_interval;
  /**Slave latency for the connection in number of connection events. Range: 0x0000 to 0x01F3*/
  uint16_t conn_latency;
  /**Connection supervision timeout. Range: 0x000A to 0x0C80*/
  uint16_t supervision_timeout;
  /**Only applicable for slave ,for master this value is set to 0x00*/
  uint8_t master_clock_accuracy;
  /**Status of the Connection - success/failure*/
  uint8_t status;
} rsi_ble_event_enhance_conn_status_t;

//Disconnect event structure
typedef struct rsi_ble_event_disconnect_s {
  /**device address of the disconnected device*/
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
  /**The type of the disconnected device -(Classic/LE) */
  uint8_t dev_type;

} rsi_ble_event_disconnect_t;

//le ping timeout expired event structure
typedef struct rsi_ble_event_le_ping_time_expired_s {
  /**Device address of the disconnected device*/
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];

} rsi_ble_event_le_ping_time_expired_t;

//le ltk request event Structure
typedef struct rsi_bt_event_le_ltk_request_s {
  /**BD Address of the remote device*/
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
  /**ediv of local device*/
  uint16_t localediv;
  /**rand of local device*/
  uint8_t localrand[8];
  /**Address type of remote device*/
  uint8_t dev_addr_type;
} rsi_bt_event_le_ltk_request_t;

//le security keys event Structure
typedef struct rsi_bt_event_le_security_keys_s {
  /**BD Address of the remote device*/
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
  /**16 byte irk of the local device*/
  uint8_t local_irk[16];
  /**16 byte irk of the remote device*/
  uint8_t remote_irk[16];
  /**remote device ediv value*/
  uint16_t remote_ediv;
  /**remote device rand value*/
  uint8_t remote_rand[16];
  /**remote device ltk value*/
  uint8_t remote_ltk[16];
  /**Identity address type - public/random \n
      0x00 --> Public Identity Address \n
      0x01 --> Random (static) Identity Address \n
      All other values Reserved for future use*/
  uint8_t Identity_addr_type;
  /**Identity address which is resolved after security keys exchange*/
  uint8_t Identity_addr[6];
  /**Device address type*/
  uint8_t dev_addr_type;
} rsi_bt_event_le_security_keys_t;

//encryption enabled structure
typedef struct rsi_bt_event_encryption_enabled_s {
  /**Remote device Address*/
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
  /**Status of the Encryption \n
      ENCRYPT_ENABLED BIT(0) --> To indicate or set encryption is enabled AUTH_LTK_OR_STK_ENC \n
	  BIT(1) --> To indicate or set Authenticated Pairing and Encryption UN_AUTH_LTK_OR_STK_ENC \n
	  BIT(2) --> To indicate or set UnAuthenticated pairing and Encryption AUTH_LTK_WITH_LE_SC_ENC \n
	  BIT(3) --> To indicate or set Authenticated Pairing and Enc with LE SC */
  uint8_t enabled;
  /**BLE Secure Connections Enable/disable indication \n
      0 --> Disable \n
      1 --> Enable */
  uint8_t sc_enable;
  /**Local device EDIV*/
  uint16_t localediv;
  /**Local RAND*/
  uint8_t localrand[8];
  /**Local Long term Key*/
  uint8_t localltk[16];
  /**Remote Device Address type*/
  uint8_t dev_addr_type;
} rsi_bt_event_encryption_enabled_t;

//SMP protocol structure
//SMP Request event structure
typedef struct rsi_bt_event_smp_req_s {
  /**address of remote device*/
  uint8_t dev_addr[6];
} rsi_bt_event_smp_req_t;

//SMP Response event structure
typedef struct rsi_bt_event_smp_resp_s {
  /**address of remote device*/
  uint8_t dev_addr[6];
  /**Device input output capability \n
      0x00 - Display Only \n
      0x01 - Display Yes/No \n
      0x02 - Keyboard Only  \n
      0x03 - No Input No Output
      0x04 - Keyboard Dispaly*/
  uint8_t io_cap;
  /**Out Of the Band data*/
  uint8_t oob_data;
  /**Authentication Request contains bonding type \n
      MITM Request - BIT(2) \n
      Secure Connections - BIT(3) \n
      Keypress - BIT(4) \n
      CT2 - BIT(5)*/
  uint8_t auth_req;
  /**Minimum required key size*/
  uint8_t min_req_key_size;
  /** Initiator generates/requires the no .of keys after successful paring \n
      BIT(0) - EncKey: Initiator shall distribute LTK followed by EDIV and Rand \n
      BIT(1) - IdKey : Initiator shall distribute IRK followed by its address \n
      BIT(2) - Sign  : Initiator shall distribute CSRK \n
      BIT(3) - BIT(7): Reserved for future use */
  uint8_t ini_key_distrb;
  /** Responder generates/requires the no .of keys after successful paring \n
      BIT(0) - EncKey: Responder shall distribute LTK followed by EDIV and Rand \n
      BIT(1) - IdKey : Responder shall distribute IRK followed by its address \n
      BIT(2) - Sign  : Responder shall distribute CSRK \n
      BIT(3) - BIT(7): Reserved for future use */
  uint8_t resp_key_distrb;
} rsi_bt_event_smp_resp_t;

//SMP passkey event structure
typedef struct rsi_bt_event_smp_passkey_s {
  /**address of remote device*/
  uint8_t dev_addr[6];
} rsi_bt_event_smp_passkey_t;

//SMP passkey display event structure
typedef struct rsi_bt_event_smp_passkey_display_s {
  /**address of remote device*/
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
  /**This is the key required in pairing process( 6 bytes)*/
  uint8_t passkey[6];
} rsi_bt_event_smp_passkey_display_t;

//SMP passkey display event structure
typedef struct rsi_bt_event_sc_passkey_s {
  /**address of remote device*/
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
  uint8_t reserved[2];
  /**This is the key required in pairing process*/
  uint32_t passkey;
} rsi_bt_event_sc_passkey_t;

//SMP failed event structure
typedef struct rsi_bt_event_smp_failed_s {
  /**device address of the disconnected device*/
  uint8_t dev_addr[6];
} rsi_bt_event_smp_failed_t;

//Security Methods event structure
typedef struct rsi_bt_event_sc_method_s {
  /**Security Method --> Justworks or Passkey \n
     RSI_BT_LE_SC_JUST_WORKS		0x01 \n
     RSI_BT_LE_SC_PASSKEY   		0x02 */
  uint8_t sc_method;
} rsi_bt_event_sc_method_t;

// phy update complete event
typedef struct rsi_ble_event_phy_update_s {
  /**Device address of the remote device.*/
  uint8_t dev_addr[6];
  /**Transmitission phy rate(1 byte) \n
     BIT(0) - The Host prefers to use the LE 1M transmitter PHY (possibly among others) \n
     BIT(1) - The Host prefers to use the LE 2M transmitter PHY (possibly among others) \n
     BIT(2) - The Host prefers to use the LE Coded transmitter PHY (possibly among others) \n
     BIT(3) - BIT(7) Reserved for future use */
  uint8_t TxPhy;
  /**Reception phy rate(1 byte) \n
     BIT(0) - The Host prefers to use the LE 1M transmitter PHY (possibly among others) \n
     BIT(1) - The Host prefers to use the LE 2M transmitter PHY (possibly among others) \n
     BIT(2) - The Host prefers to use the LE Coded transmitter PHY (possibly among others) \n
     BIT(3) - BIT(7) Reserved for future use */
  uint8_t RxPhy;
} rsi_ble_event_phy_update_t;

// conn update complete event
typedef struct rsi_ble_event_conn_update_s {
  /**Device address of the remote device*/
  uint8_t dev_addr[6];
  /**Connection Interval*/
  uint16_t conn_interval;
  /**Connection Latency*/
  uint16_t conn_latency;
  /**Supervision Timeout*/
  uint16_t timeout;
} rsi_ble_event_conn_update_t;

// remote conn params request event  //event_id : 0x152E
typedef struct rsi_ble_event_remote_conn_param_req_s {
  /**Device address of the remote device*/
  uint8_t dev_addr[6];
  /**Minimum connection interval*/
  uint16_t conn_interval_min;
  /**Maximum connection interval*/
  uint16_t conn_interval_max;
  /**Connection Latency*/
  uint16_t conn_latency;
  /**Supervision Timeout*/
  uint16_t timeout;
} rsi_ble_event_remote_conn_param_req_t;

// remote features event
typedef struct rsi_ble_event_remote_features_s {
  /**Remote device address*/
  uint8_t dev_addr[6];
  /**Remote device supported features \n
     @note please refer spec for the supported features list */
  uint8_t remote_features[8];
} rsi_ble_event_remote_features_t;

// LE Device Buffer Indication
typedef struct rsi_ble_event_le_dev_buf_ind_s {
  /**Remote device address*/
  uint8_t remote_dev_bd_addr[RSI_DEV_ADDR_LEN];
  /**No. of Available buffer*/
  uint8_t avail_buf_cnt;
} rsi_ble_event_le_dev_buf_ind_t;

// phy update complete event
typedef struct rsi_ble_event_data_length_update_s {
  /**Device address of the remote device.*/
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
  /**Maximum Tx Octets to be transmitted*/
  uint16_t MaxTxOctets;
  /**Maximu Tx time to transmit the MaxTxOctets*/
  uint16_t MaxTxTime;
  /**Maximum Rx Octets to be received*/
  uint16_t MaxRxOctets;
  /**Maximum Rx time to receive the MaxRxOctets*/
  uint16_t MaxRxTime;
} rsi_ble_event_data_length_update_t;

// Basic GATT structures

// 128 bit UUID format structure
typedef struct uuid128_s {

  uint32_t data1;
  uint16_t data2;
  uint16_t data3;
  uint8_t data4[8];
} uuid128_t;

// 16 bit UUID format structure
typedef uint16_t uuid16_t;

// 32 bit UUID format structure
typedef uint32_t uuid32_t;

// UUID format structure
typedef struct uuid_s {
  /** Size of uuid */
  uint8_t size;
  /** Reserved */
  uint8_t reserved[3];
  /** Value of one of the 3 types (128 bit, 32 bit or 16 bit) of UUIDs. */
  union uuid_t {
    /** uint8_t[16], 128 bit(16 byte) UUID value*/
    uuid128_t val128;
    /**uint8_t[4], 32 bit(4 bytes) UUID value*/
    uuid32_t val32;
    /**uint8_t[2], 16 bit(2 bytes) UUID value*/
    uuid16_t val16;
  } val;
} uuid_t;

// profile descriptor/primary service structure
typedef struct profile_descriptor_s {
  /** Start handle. */
  uint8_t start_handle[2];
  /** End handle. */
  uint8_t end_handle[2];
  /** profile uuid. */
  uuid_t profile_uuid;
} profile_descriptors_t;

// GATT attribute descriptor structure
typedef struct att_desc_s {
  /**attribute handle*/
  uint8_t handle[2];
  uint8_t reserved[2];
  /**attribute uuid (attribute type)*/
  uuid_t att_type_uuid;
} att_desc_t;

//characteristic service attribute value structure
typedef struct char_serv_data_s {
  /**characteristic value property*/
  uint8_t char_property;
  uint8_t reserved;
  /**characteristic value handle*/
  uint16_t char_handle;
  /**characteristic value attributes uuid*/
  uuid_t char_uuid;
} char_serv_data_t;

//characteristic service attribute structure
typedef struct char_serv_s {
  /**characteristic service attribute handle*/
  uint16_t handle;
  uint8_t reserved[2];
  /**characteristic service attribute value*/
  char_serv_data_t char_data;
} char_serv_t;

//include service attribute value structure
typedef struct inc_serv_data_s {
  /**include service start handle*/
  uint16_t start_handle;
  /**include service end handle*/
  uint16_t end_handle;
  /**UUID value of the included service*/
  uuid_t uuid;
} inc_serv_data_t;

// include service attribute structure
typedef struct inc_serv_s {
  /**include service attribute handle*/
  uint16_t handle;
  uint8_t reserved[2];
  /**include service attribute data structure*/
  inc_serv_data_t inc_serv;
} inc_serv_t;

// GATT Request structures

// add new attributes to service request structure
typedef struct rsi_ble_req_add_att_s {
  /** service handler */
  void *serv_handler;
  /** Attribute handle */
  uint16_t handle;
  /** If this variable is 1, Host has to maintain attributes and records in the application. \n
      If 0, Stack will maintain the attributes and records */
  uint16_t config_bitmap;
  /** Attribute type UUID */
  uuid_t att_uuid;
  /** Attribute property */
  uint8_t property;
  /** Attribute data length */
  uint16_t data_len;
  /** Attribute data. The maximum value is 240, please refer RSI_DEV_ATT_LEN Macro*/
  uint8_t data[RSI_DEV_ATT_LEN];
} rsi_ble_req_add_att_t;

// GATT Response structures

//Presentation Format descriptor structure
// actual value =  char value * 10 ^ exponent;
typedef struct rsi_ble_presentation_format {
  /**Format of the data*/
  uint8_t format;
  /**Exponent of the data*/
  uint8_t exponent;
  /**uints of the data*/
  uint16_t unit;
  /**name space of the data*/
  uint8_t name_space;
  /**Description for data*/
  uint16_t description;
} rsi_ble_pesentation_format_t;

//RSI_BLE_EVENT_GATT_ERROR_RESP, event_id: 0x1500
typedef struct rsi_ble_event_error_resp_s {
  /**remote device address*/
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
  /**attribute handle*/
  uint8_t handle[2];
  /**Error indicates the type of Gatt Error*/
  uint8_t error[2];
} rsi_ble_event_error_resp_t;

//RSI_BLE_EVENT_GATT_CHAR_DESC - event_ix = 1501
typedef struct rsi_ble_event_gatt_desc_s {
  /**remote device address*/
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
  /**number of descriptors found*/
  uint8_t num_of_att;
  uint8_t reserved;
  /**Attribute descriptors list. The maximum value is 5*/
  att_desc_t att_desc[RSI_BLE_MAX_RESP_LIST];
} rsi_ble_event_gatt_desc_t;

//RSI_BLE_EVENT_GATT_PRIMARY_SERVICE_LIST, event_id: 0x1509
typedef struct rsi_ble_event_profiles_list_s {
  /**remote device address*/
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
  /**number of profiles found*/
  uint8_t number_of_profiles;
  uint8_t reserved;
  /**list of found profiles. The maximum value is 5*/
  profile_descriptors_t profile_desc[RSI_BLE_MAX_RESP_LIST];
} rsi_ble_event_profiles_list_t;
//RSI_BLE_EVENT_GATT_PRIMARY_SERVICE_BY_UUID, event_id = 0x1502
typedef struct rsi_ble_event_profile_by_uuid_s {
  /**remote device address*/
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
  /**profile start handle*/
  uint8_t start_handle[2];
  /**profile end handle*/
  uint8_t end_handle[2];
} rsi_ble_event_profile_by_uuid_t;

//RSI_BLE_EVENT_GATT_READ_CHAR_SERVS, event_id = 0x1503
typedef struct rsi_ble_event_read_by_type1_s {
  /**remote device address*/
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
  /**number of characteristic services found*/
  uint8_t num_of_services;
  uint8_t reserved;
  /**It contains the characteristic service list. The maximum value is 5*/
  char_serv_t char_services[RSI_BLE_MAX_RESP_LIST];
} rsi_ble_event_read_by_type1_t;

//RSI_BLE_EVENT_GATT_READ_INC_SERVS, event_id = 0x1504
typedef struct rsi_ble_event_read_by_type2_s {
  /**remote device address*/
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
  /**number of characteristic services found*/
  uint8_t num_of_services;
  uint8_t reserved;
  /**list of included services. The maximum value is 5*/
  inc_serv_t services[RSI_BLE_MAX_RESP_LIST];
} rsi_ble_event_read_by_type2_t;

//RSI_BLE_EVENT_GATT_READ_VAL_BY_UUID, event_id = 0x1505
typedef struct rsi_ble_event_read_by_type3_s {
  /**remote device address*/
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
  /**attribute handle*/
  uint8_t handle[2];
  /**length of attribute value*/
  uint16_t length;
  /**This contains the attribute value. The maximum value is 240, please refer RSI_DEV_ATT_LEN Macro*/
  uint8_t att_value[RSI_DEV_ATT_LEN];
} rsi_ble_event_read_by_type3_t;

//RSI_BLE_EVENT_GATT_READ_RESP , evet_id = 0x1506,0x1507,0x1508
typedef struct rsi_ble_event_att_value_s {
  /**remote device address*/
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
  /**length of attribute value*/
  uint16_t length;
  /**This contains the attribute value. The maximum value is 240, please refer RSI_DEV_ATT_LEN Macro*/
  uint8_t att_value[RSI_DEV_ATT_LEN];
} rsi_ble_event_att_value_t;

//RSI_BLE_EVENT_GATT_WRITE_RESP, event_id: 0x150A,0x150C
typedef struct rsi_ble_set_att_resp_s {
  /**remote device address*/
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
} rsi_ble_set_att_resp_t;

//RSI_BLE_EVENT_GATT_PREPARE_WRITE_RESP, event_id: 0x150B
typedef struct rsi_ble_prepare_write_resp_s {
  /**remote device address*/
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
  /**attribute handle*/
  uint8_t handle[2];
  /**attribute value offset*/
  uint8_t offset[2];
  /**length of attribute value*/
  uint16_t length;
  /**This contains the attribute value. The maximum value is 240, please refer RSI_DEV_ATT_LEN Macro*/
  uint8_t att_value[RSI_DEV_ATT_LEN];
} rsi_ble_prepare_write_resp_t;

// GATT Profiles list response structure
typedef struct rsi_ble_resp_profiles_list_s {
  /** Number of profiles found */
  uint8_t number_of_profiles;
  /** Reserved */
  uint8_t reserved[3];
  /** List of found profiles \n
      The maximum value is 5 */
  profile_descriptors_t profile_desc[RSI_BLE_MAX_RESP_LIST];
} rsi_ble_resp_profiles_list_t;

typedef struct rsi_ble_resp_query_profile_descriptor_s {
  /**remote device address*/
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
  /**List of found profiles \n
      The maximum value is 5 */
  profile_descriptors_t profile_desc[RSI_BLE_MAX_RESP_LIST];
} rsi_ble_resp_query_profile_descriptor_t;

// GATT characteristic query service response structure
typedef struct rsi_ble_resp_char_serv_s {
  /** The number of profiles found */
  uint8_t num_of_services;
  /** Reserved */
  uint8_t reserved[3];
  /** Characteristic service array. \n
      The maximum value is 5. */
  char_serv_t char_services[RSI_BLE_MAX_RESP_LIST];
} rsi_ble_resp_char_services_t;

// GATT include service response structure
typedef struct rsi_ble_resp_inc_serv {
  /** Number of profiles found */
  uint8_t num_of_services;
  /** Reserved */
  uint8_t reserved[3];
  /** Include service list. \n
      The maximum value is 5. */
  inc_serv_t services[RSI_BLE_MAX_RESP_LIST];
} rsi_ble_resp_inc_services_t;

// GATT attribute value response structure
typedef struct rsi_ble_resp_att_value_s {
  /** Length of attribute value */
  uint8_t len;
  /** Attribute values list. \n 
      Each attribute value is maximum of size 240, please refer RSI_DEV_ATT_LEN Macro */
  uint8_t att_value[RSI_DEV_ATT_LEN];
} rsi_ble_resp_att_value_t;

// GATT attribute descriptors response structure
typedef struct rsi_ble_resp_att_descs_s {
  /** Number of descriptors found */
  uint8_t num_of_att;
  /** Reserved */
  uint8_t reserved[3];
  /** Attribute descriptors list. \n
      The maximum value is 5. */
  att_desc_t att_desc[RSI_BLE_MAX_RESP_LIST];
} rsi_ble_resp_att_descs_t;

// add new service response structure
typedef struct rsi_ble_resp_add_serv_s {
  /** Contains the address of the service record stored in the Silicon Labs stack. */
  void *serv_handler;
  /** The handle from where the service starts. */
  uint16_t start_handle;
} rsi_ble_resp_add_serv_t;

// read local attribute value response structure
typedef struct rsi_ble_resp_local_att_value_s {
  /** Attribute handle */
  uint16_t handle;
  /** Attribute value length */
  uint16_t data_len;
  /** Attribute value (data). The maximum value is 240, please refer RSI_DEV_ATT_LEN Macro */
  uint8_t data[RSI_DEV_ATT_LEN];
} rsi_ble_resp_local_att_value_t;

// GATT Event structures

// GATT Write event structure
typedef struct rsi_ble_event_write_s {
  /**remote device address*/
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
  uint8_t reserved;
#define RSI_BLE_WRITE_CMD_EVENT     0x01
#define RSI_BLE_WRITE_REQUEST_EVENT 0x02
#define RSI_BLE_NOTIFICATION_EVENT  0x03
#define RSI_BLE_INDICATION_EVENT    0x04
  /**Type of the event received from the remote device \n
     RSI_BLE_WRITE_CMD_EVENT     0x01 \n
     RSI_BLE_WRITE_REQUEST_EVENT 0x02 \n
     RSI_BLE_NOTIFICATION_EVENT  0x03 \n
     RSI_BLE_INDICATION_EVENT    0x04 */
  uint8_t pkt_type;
  /**attribute handle*/
  uint8_t handle[2];
  /**length of attribute value*/
  uint8_t length;
  /**This contains the attribute value. The maximum value is 240, please refer RSI_DEV_ATT_LEN Macro*/
  uint8_t att_value[RSI_DEV_ATT_LEN];
} rsi_ble_event_write_t;

// GATT prepare Write event structure
typedef struct rsi_ble_event_prepare_write_s {
  /**remote device address*/
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
  /**attribute handle*/
  uint8_t handle[2];
  /**attribute value offset*/
  uint8_t offset[2];
  /**length of attribute value*/
  uint16_t length;
  /**This contains the attribute value. The maximum value is 240, please refer RSI_DEV_ATT_LEN Macro*/
  uint8_t att_value[RSI_DEV_ATT_LEN];
} rsi_ble_event_prepare_write_t;

// GATT execuite Write event structure
typedef struct rsi_ble_execute_write_s {
  //uint8_t[6], remote device address.
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
  //uint8_t, execute write flag.
  uint8_t exeflag;
} rsi_ble_execute_write_t;

// GATT execuite Write event structure
typedef struct rsi_ble_read_req_s {
  /**remote device address*/
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
  /**attribute handle*/
  uint16_t handle;
  /**0 � Read request \n 
     1 � Read Blob request*/
  uint8_t type;
  uint8_t reserved;
  /**offset of attribute value to be read*/
  uint16_t offset;
} rsi_ble_read_req_t;

// GATT execuite Write event structure
typedef struct rsi_ble_event_mtu_s {
  /**remote device address*/
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
  /**MTU size*/
  uint16_t mtu_size;
} rsi_ble_event_mtu_t;

#define PEER_DEVICE_INITATED_MTU_EXCHANGE  0x1
#define LOCAL_DEVICE_INITATED_MTU_EXCHANGE 0x2
//MTU Exchange Information event structure
typedef struct rsi_ble_event_mtu_exchange_information_s {
  /**uint8_t[6], remote device address*/
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
  /**uint8_t[2], Remote MTU Size*/
  uint16_t remote_mtu_size;
  /**uint8_t[2], Local MTU Size*/
  uint16_t local_mtu_size;
  /**uint8_t Initiated role, who initiated mtu exchange \n
     PEER_DEVICE_INITATED_MTU_EXCHANGE     0x01 \n
     LOCAL_DEVICE_INITATED_MTU_EXCHANGE    0x02  */
  uint8_t initiated_role;
} rsi_ble_event_mtu_exchange_information_t;
// GATT Notification event structure
typedef struct rsi_ble_event_notify_s {
  /**remote device address*/
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
  /**attribute handle*/
  uint8_t handle[2];
  /**length of attribute value*/
  uint8_t length;
  /**This contains the attribute value. The maximum value is 240, please refer RSI_DEV_ATT_LEN Macro*/
  uint8_t att_value[RSI_DEV_ATT_LEN];
} rsi_ble_event_notify_t;

// GATT Indication event structure
typedef struct rsi_ble_event_indication_s {
  /**remote device address*/
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
  /**attribute handle*/
  uint8_t handle[2];
  /**length of attribute value*/
  uint8_t length;
  /**This contains the attribute value. The maximum value is 240, please refer RSI_DEV_ATT_LEN Macro*/
  uint8_t att_value[RSI_DEV_ATT_LEN];
} rsi_ble_event_indication_t;

typedef struct rsi_ble_event_directedadv_report_s {
  /**Event type \n
      0x01	Connectable directed advertising (ADV_DIRECT_IND) */
  uint16_t event_type;
  /**Address type of remote device*/
  uint8_t dev_addr_type;
  /**Address of the remote device*/
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
  /**Directed address type*/
  uint8_t directed_addr_type;
  /**Directed device address*/
  uint8_t directed_dev_addr[RSI_DEV_ADDR_LEN];
  /**rssi value*/
  int8_t rssi;
} rsi_ble_event_directedadv_report_t;

typedef struct rsi_ble_event_cbfc_conn_req_s {
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
  uint16_t psm;
  uint16_t lcid;
} rsi_ble_event_cbfc_conn_req_t;

typedef struct rsi_ble_event_cbfc_conn_complete_s {
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
  uint16_t psm;
  uint16_t mtu;
  uint16_t mps;
  uint16_t lcid;
} rsi_ble_event_cbfc_conn_complete_t;

typedef struct rsi_ble_event_cbfc_rx_data_s {
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
  uint16_t lcid;
  uint16_t len;
  uint8_t data[RSI_DEV_ATT_LEN];
} rsi_ble_event_cbfc_rx_data_t;

typedef struct rsi_ble_event_cbfc_disconn_s {
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
  uint16_t lcid;
} rsi_ble_event_cbfc_disconn_t;

// SMP pairing capabilities    cmd_ix = 0x00FE
typedef struct rsi_ble_set_smp_pairing_capabilty_data {

  /**Device input output capability \n
      0x00 - Display Only \n
      0x01 - Display Yes/No \n
      0x02 - Keyboard Only  \n
      0x03 - No Input No Output
      0x04 - Keyboard Dispaly*/
  uint8_t io_capability;
  /** oob_data_flag \n
      0 - disable \n 1 - enable */
  uint8_t oob_data_flag;
  /** Authentication Request contains bonding type \n
      MITM Request - BIT(2) \n
      Secure Connections - BIT(3) \n
      Keypress - BIT(4) \n
      CT2 - BIT(5) */
  uint8_t auth_req;
  /** Supported Encryption key size 7 to 16 bytes */
  uint8_t enc_key_size;
  /** Initiator generates/requires the no .of keys after successful paring \n
      BIT(0) - EncKey: Initiator shall distribute LTK followed by EDIV and Rand \n
      BIT(1) - IdKey : Initiator shall distribute IRK followed by its address \n
      BIT(2) - Sign  : Initiator shall distribute CSRK \n
      BIT(3) - BIT(7): Reserved for future use */
  uint8_t ini_key_distribution;
  /** Responder generates/requires the no .of keys after successful paring \n
      BIT(0) - EncKey: Responder shall distribute LTK followed by EDIV and Rand \n
      BIT(1) - IdKey : Responder shall distribute IRK followed by its address \n
      BIT(2) - Sign  : Responder shall distribute CSRK \n
      BIT(3) - BIT(7): Reserved for future use */
  uint8_t rsp_key_distribution;

} rsi_ble_set_smp_pairing_capabilty_data_t;
//LE read phy request command reasponse structure, cmd_ix - 0x00B0
typedef struct rsi_ble_resp_read_phy_s {
  /** Remote device Bluetooth Address*/
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
  /** TX PHY Rate \n
       0x01 The transmitter PHY for the connection is LE 1M \n
       0x02 The transmitter PHY for the connection is LE 2M \n
       0x03 The transmitter PHY for the connection is LE Coded \n
       All other values Reserved for future use*/
  uint8_t tx_phy;
  /** RX PHY Rate \n
       0x01 The receiver PHY for the connection is LE 1M \n
       0x02 The receiver PHY for the connection is LE 2M \n
       0x03 The receiver PHY for the connection is LE Coded \n
       All other values Reserved for future use*/
  uint8_t rx_phy;
} rsi_ble_resp_read_phy_t;

//LE read max data length command reasponse structure, cmd_ix - 0x00B3
typedef struct rsi_ble_resp_read_max_data_length_s {
  /** maxtxoctets \n 
       	Preferred maximum number of payload octets that the local Controller should
        include in a single Link Layer packet on this connection. */
  uint16_t maxtxoctets;
  /** maxtxtime \n 
	    Preferred maximum number of microseconds that the local Controller
        should use to transmit a single Link Layer packet on this connection */
  uint16_t maxtxtime;
  /** maxrxoctets \n 
	    Maximum number of payload octets that the local Controller supports
        for reception of a single Link Layer packet on a data connection */
  uint16_t maxrxoctets;
  /** maxrxtime \n 
	    Maximum time, in microseconds, that the local Controller supports for
        reception of a single Link Layer packet on a data connection. */
  uint16_t maxrxtime;
} rsi_ble_read_max_data_length_t;

//LE Per Transmit mode, cmd_ix = 0x00BE
typedef struct rsi_ble_per_transmit_s {
  /** Command ID \n
   Takes per BLE_TRANSMIT_CMD_ID of value 0x13*/
  uint8_t cmd_ix;
  /** Enables/disables the ble per transmit mode \n
     1  PER Transmit Enable \n
     0  PER Transmit Disable */
  uint8_t transmit_enable;
  /** Access address with which packets are transmitted */
  uint8_t access_addr[4];
  /** Phy rate at which packets are transmitted \n
       1  1Mbps  \n
       2  2 Mbps \n
       4  125 Kbps Coded \n
       8  500 Kbps Coded */
  uint8_t phy_rate;
  /** Rx channel number (0 - 39) */
  uint8_t rx_chnl_num;
  /** Tx channel number (0 - 39) */
  uint8_t tx_chnl_num;
  /** Initial seed to be used for whitening. It should be set to �0� in order to disable whitening. \n
      In order to enable, one should give the scrambler seed value which is used on the receive side */
  uint8_t scrambler_seed;
  /** LE channel type (data or advertise channel) \n
       0x00  Advertise Channel \n
       0x01  Data Channel (to be used by Default) */
  uint8_t le_chnl_type;
  /** Frequency hopping type to be used \n
       0  No Hopping \n
       1  Fixed Hopping \n
       2  Random Hopping (rx_chnl_num, tx_chnl_num parameters are unused in this mode) */
  uint8_t freq_hop_en;
  /** Antenna selection (onboard/external) to be used for reception \n 
       2  ONBOARD_ANT_SEL \n
       3  EXT_ANT_SEL */
  uint8_t ant_sel;
  /** pll_mode type to be used \n 
       0  PLL_MODE0 (to be used by Default) \n 
       1  PLL_MODE1 */
  uint8_t pll_mode;
  /** Selection of RF type (internal/external) \n
       0  BT_EXTERNAL_RF \n
       1  BT_INTERNAL_RF (to be used by Default) 
      @note  The above macros are applicable for both BT and BLE */
  uint8_t rf_type;
  /** Selection of RF Chain (HP/LP) to be used \n
       0  BT_HP_CHAIN \n
       1  BT_LP_CHAIN 
      @note  The above macros are applicable for both BT and BLE */
  uint8_t rf_chain;
  /** Length of the packet to be transmitted*/
  uint8_t pkt_len[2];
  /** Type of payload data sequence \n
       0x00  PRBS9 sequence �11111111100000111101... \n
       0x01  Repeated �11110000� \n
       0x02  Repeated �10101010� \n
       0x03  PRBS15 \n
       0x04  Repeated �11111111� \n
       0x05  Repeated �00000000� \n
       0x06  Repeated '00001111' \n
       0x07  Repeated '01010101' */
  uint8_t payload_type;
  /** Transmit Power \n 
       Transmit power value for the rf_chain parameter set to the HP chain the values for the tx power indexes are 0 - 20. \n 
       Transmit power value for the rf chain parameter set to LP chain and values are: \n
       0 -31 o/p power equation is  -2+10log10(power_index/31) \n
       32-63 o/p power equation is  \n
       6 + 10log10((power_index -32)/31) \n
       TX power for the BLE LP Chain :1 to 31 (0dBm Mode), 33 to 63 ( 10dBm Mode) \n
       TX power for the BLE HP chain : 64 to 79 */
  uint8_t tx_power;
  /** Transmit mode to be used either Burst/Continuous \n
       0  BURST_MODE \n
       1  CONTINUOUS_MODE \n
       2  CONTINUOUS_WAVE_MODE (CW_MODE) */
  uint8_t transmit_mode;
  /** This field takes the value of inter packet gap. \n
      Number of slots to be skipped between two packets - Each slot will be 1250usec */
  uint8_t inter_pkt_gap;
  /** This field defines the number of packets to be transmitted, default to zero for continuous transmission */
  uint8_t num_pkts[4];
} rsi_ble_per_transmit_t;

//LE Per Receive mode, cmd_ix = 0x00BF
typedef struct rsi_ble_per_receive_s {
  /** Command ID \n
   Takes per BLE_RECEIVE_CMD_ID of value 0x14*/
  uint8_t cmd_ix;
  /** Enables/disables the ble per receive mode \n
     1  PER Receive Enable \n
     0  PER Receive Disable */
  uint8_t receive_enable;
  /** Access address with which packets are received */
  uint8_t access_addr[4];
  /** Phy rate at which packets are received \n
       1  1Mbps  \n
       2  2 Mbps \n
       4  125 Kbps Coded \n
       8  500 Kbps Coded */
  uint8_t phy_rate;
  /** Rx channel number (0 - 39) */
  uint8_t rx_chnl_num;
  /** Tx channel number (0 - 39) */
  uint8_t tx_chnl_num;
  /** Initial seed to be used for whitening. It should be set to �0� in order to disable whitening. \n
      In order to enable, one should give the scrambler seed value which is used on the transmit side */
  uint8_t scrambler_seed;
  /** LE channel type (data or advertise channel) \n
       0x00  Advertise Channel \n
       0x01  Data Channel (to be used by Default) */
  uint8_t le_chnl_type;
  /** Frequency hopping type to be used \n
       0  No Hopping \n
       1  Fixed Hopping \n
       2  Random Hopping (rx_chnl_num, tx_chnl_num parameters are unused in this mode) */
  uint8_t freq_hop_en;
  /** Antenna selection (onboard/external) to be used for reception \n 
       2  ONBOARD_ANT_SEL \n
       3  EXT_ANT_SEL */
  uint8_t ant_sel;
  /** pll_mode type to be used \n 
       0  PLL_MODE0 (to be used by Default) \n 
       1  PLL_MODE1 */
  uint8_t pll_mode;
  /** Selection of RF type (internal/external) \n
       0  BT_EXTERNAL_RF \n
       1  BT_INTERNAL_RF (to be used by Default) 
      @note  The above macros are applicable for both BT and BLE */
  uint8_t rf_type;
  /** Selection of RF Chain (HP/LP) to be used \n
       0  BT_HP_CHAIN \n
       1  BT_LP_CHAIN 
      @note  The above macros are applicable for both BT and BLE */
  uint8_t rf_chain;
  /** This field enables/disables the extended data length \n
       0  Extended Data length disabled \n
       1  Extended Data length enabled */
  uint8_t ext_data_len_indication;
  /** This field defines the loopback to be enable or disable \n
       0  LOOP_BACK_MODE_DISABLE \n
       1  LOOP_BACK_MODE_ENABLE*/
  uint8_t loop_back_mode;
  /** This field enables/disables the duty cycling \n
       0  Duty Cycling Disabled (to be used by Default) \n
       1  Duty Cycling Enabled */
  uint8_t duty_cycling_en;
} rsi_ble_per_receive_t;
/******************************************************
 * *                 Global Variables
 * ******************************************************/

typedef struct chip_ble_buffers_stats_s {
  uint8_t acl_buffer_utilization;
  uint8_t cp_buffer_utilization;
} chip_ble_buffers_stats_t;

/******************************************************
 * *              GAP API's Declarations
 * ******************************************************/

//*==============================================*/
/**
 * @fn         rsi_convert_db_to_powindex
 * */
uint8_t rsi_convert_db_to_powindex(int8_t tx_power_in_dBm);

//*==============================================*/
/**
 * @fn         rsi_ble_start_advertising
 * */
int32_t rsi_ble_start_advertising(void);

/*==============================================*/
/**
 * @fn         rsi_ble_start_advertising_with_values
 * */
int32_t rsi_ble_start_advertising_with_values(void *rsi_ble_adv);

/**
 * @fn         rsi_ble_update_direct_address
 * @brief      request for update direct address 
 * @param[in]  remote_dev_addr
 * @return     void
 * @section description
 * This function requests for update direct address 
 * */
void rsi_ble_update_directed_address(uint8_t *remote_dev_addr);

/*==============================================*/
/**
 * @fn         rsi_ble_encrypt
 * */
int32_t rsi_ble_encrypt(uint8_t *, uint8_t *, uint8_t *);

/*==============================================*/
/**
 * @fn         rsi_ble_stop_advertising
 * */
int32_t rsi_ble_stop_advertising(void);

/*==============================================*/
/**
 * @fn         rsi_ble_set_advertise_data
 * */
int32_t rsi_ble_set_advertise_data(uint8_t *data, uint16_t data_len);

/*========================================================*/
/**
 * @fn			rsi_ble_set_scan_response_data
 * */
int32_t rsi_ble_set_scan_response_data(uint8_t *data, uint16_t data_len);

/*==============================================*/
/**
 * @fn         rsi_ble_start_scanning
 * */
int32_t rsi_ble_start_scanning(void);

/*==============================================*/
/**
 * @fn         rsi_ble_start_scanning_with_values
 * */
int32_t rsi_ble_start_scanning_with_values(void *rsi_ble_scan_params);

/*==============================================*/
/**
 * @fn         rsi_ble_stop_scanning
 * */
int32_t rsi_ble_stop_scanning(void);

/*==============================================*/
/**
 * @fn         rsi_ble_connect
 * */
int32_t rsi_ble_connect(uint8_t remote_dev_addr_type, int8_t *remote_dev_addr);

/*==============================================*/
/**
 * @fn         rsi_ble_connect_with_params
 * */
int32_t rsi_ble_connect_with_params(uint8_t remote_dev_addr_type,
                                    int8_t *remote_dev_addr,
                                    uint16_t scan_interval,
                                    uint16_t scan_window,
                                    uint16_t conn_interval_max,
                                    uint16_t conn_interval_min,
                                    uint16_t conn_latency,
                                    uint16_t supervision_tout);

/*==============================================*/
/**
 * @fn         rsi_ble_connect_cancel
 * 
 * */
int32_t rsi_ble_connect_cancel(int8_t *remote_dev_addr);

/*==============================================*/
/**
 * @fn         rsi_ble_disconnect
 * */
int32_t rsi_ble_disconnect(int8_t *remote_dev_addr);

/*==============================================*/
/**
 * @fn         rsi_ble_get_device_state
 */
int32_t rsi_ble_get_device_state(uint8_t *resp);

/*==============================================*/
/**
 * @fn         rsi_ble_add_dev_to_resolvlist
 *
 */
int32_t rsi_ble_resolvlist(uint8_t process_type,
                           uint8_t remote_dev_addr_type,
                           uint8_t *remote_dev_address,
                           uint8_t *peer_irk,
                           uint8_t *local_irk);

/*==============================================*/
/**
 * @fn         rsi_ble_get_resolving_list_size
 */
int32_t rsi_ble_get_resolving_list_size(uint8_t *resp);
/*==============================================*/
/**
 * @fn         rsi_ble_set_addr_resolution_enable
 */
int32_t rsi_ble_set_addr_resolution_enable(uint8_t enable, uint16_t tout);
/*==============================================*/
/**
 * @fn         rsi_ble_ltk_req_reply
 */
int32_t rsi_ble_ltk_req_reply(uint8_t *remote_dev_address, uint8_t reply_type, uint8_t *ltk);
/*==============================================*/
/**
 * @fn         rsi_ble_set_privacy_mode
 */
int32_t rsi_ble_set_privacy_mode(uint8_t remote_dev_addr_type, uint8_t *remote_dev_address, uint8_t privacy_mode);
/******************************************************
 * *        GATT Client API's Declarations
 * ******************************************************/
/*==============================================*/
/**
 * @fn         rsi_ble_get_profiles_async
 */

int32_t rsi_ble_get_profiles_async(uint8_t *dev_addr,
                                   uint16_t start_handle,
                                   uint16_t end_handle,
                                   rsi_ble_resp_profiles_list_t *p_prof_list);

/*==============================================*/
/**
 * @fn         rsi_ble_get_profile_async
 */
int32_t rsi_ble_get_profile_async(uint8_t *dev_addr, uuid_t profile_uuid, profile_descriptors_t *p_profile);

/*==============================================*/
/**
 * @fn         rsi_ble_get_char_services_async 
 */
int32_t rsi_ble_get_char_services_async(uint8_t *dev_addr,
                                        uint16_t start_handle,
                                        uint16_t end_handle,
                                        rsi_ble_resp_char_services_t *p_char_serv_list);
/*==============================================*/
/**
 * @fn         rsi_ble_get_inc_services_async  
 */
int32_t rsi_ble_get_inc_services_async(uint8_t *dev_addr,
                                       uint16_t start_handle,
                                       uint16_t end_handle,
                                       rsi_ble_resp_inc_services_t *p_inc_serv_list);
/*==============================================*/
/**
 * @fn         rsi_ble_get_char_value_by_uuid_async
 */
int32_t rsi_ble_get_char_value_by_uuid_async(uint8_t *dev_addr,
                                             uint16_t start_handle,
                                             uint16_t end_handle,
                                             uuid_t char_uuid,
                                             rsi_ble_resp_att_value_t *p_char_val);
/*==============================================*/
/**
 * @fn         rsi_ble_get_att_descriptors_async 
  */
int32_t rsi_ble_get_att_descriptors_async(uint8_t *dev_addr,
                                          uint16_t start_handle,
                                          uint16_t end_handle,
                                          rsi_ble_resp_att_descs_t *p_att_desc);
/*==============================================*/
/**
 * @fn         rsi_ble_get_att_value_async 
 */
int32_t rsi_ble_get_att_value_async(uint8_t *dev_addr, uint16_t handle, rsi_ble_resp_att_value_t *p_att_val);
/*==============================================*/
/**
 * @fn         rsi_ble_get_multiple_att_values_async 
 */

int32_t rsi_ble_get_multiple_att_values_async(uint8_t *dev_addr,
                                              uint8_t num_of_handlers,
                                              uint16_t *handles,
                                              rsi_ble_resp_att_value_t *p_att_vals);
/*==============================================*/
/**
 * @fn         rsi_ble_get_long_att_value_async 
 */

int32_t rsi_ble_get_long_att_value_async(uint8_t *dev_addr,
                                         uint16_t handle,
                                         uint16_t offset,
                                         rsi_ble_resp_att_value_t *p_att_vals);

/*==============================================*/
/**
 * @fn         rsi_ble_set_att_value_async 
 */
int32_t rsi_ble_set_att_value_async(uint8_t *dev_addr, uint16_t handle, uint8_t data_len, uint8_t *p_data);

/*==============================================*/
/**
 * @fn         rsi_ble_prepare_write_async  
 */

int32_t rsi_ble_prepare_write_async(uint8_t *dev_addr,
                                    uint16_t handle,
                                    uint16_t offset,
                                    uint8_t data_len,
                                    uint8_t *p_data);
/*==============================================*/
/**
 * @fn         rsi_ble_execute_write_async  
 */

int32_t rsi_ble_execute_write_async(uint8_t *dev_addr, uint8_t exe_flag);

/**
 * @fn         rsi_ble_get_profiles
 *
 * 
 */
int32_t rsi_ble_get_profiles(uint8_t *dev_addr,
                             uint16_t start_handle,
                             uint16_t end_handle,
                             rsi_ble_resp_profiles_list_t *p_prof_list);

/**
 * @fn         rsi_ble_get_profile
 * 
 */
int32_t rsi_ble_get_profile(uint8_t *dev_addr, uuid_t profile_uuid, profile_descriptors_t *p_profile);

/**
 * @fn         rsi_ble_get_char_services
 *
 * 
 */
int32_t rsi_ble_get_char_services(uint8_t *dev_addr,
                                  uint16_t start_handle,
                                  uint16_t end_handle,
                                  rsi_ble_resp_char_services_t *p_char_serv_list);

/**
 * @fn         rsi_ble_get_inc_services
 */
int32_t rsi_ble_get_inc_services(uint8_t *dev_addr,
                                 uint16_t start_handle,
                                 uint16_t end_handle,
                                 rsi_ble_resp_inc_services_t *p_inc_serv_list);

/**
 * @fn         rsi_ble_get_att_descriptors
 */
int32_t rsi_ble_get_att_descriptors(uint8_t *dev_addr,
                                    uint16_t start_handle,
                                    uint16_t end_handle,
                                    rsi_ble_resp_att_descs_t *p_att_desc);

/**
 * @fn         rsi_ble_get_char_value_by_uuid
 */
int32_t rsi_ble_get_char_value_by_uuid(uint8_t *dev_addr,
                                       uint16_t start_handle,
                                       uint16_t end_handle,
                                       uuid_t char_uuid,
                                       rsi_ble_resp_att_value_t *p_char_val);

/**
 * @fn         rsi_ble_get_att_value
 */
int32_t rsi_ble_get_att_value(uint8_t *dev_addr, uint16_t handle, rsi_ble_resp_att_value_t *p_att_val);

/**
 * @fn         rsi_ble_get_multiple_att_values
 */
int32_t rsi_ble_get_multiple_att(uint8_t *dev_addr,
                                 uint8_t num_handles,
                                 uint16_t *handles,
                                 rsi_ble_resp_att_value_t *p_att_vals);

/**
 * @fn         rsi_ble_get_long_att_value
 *
 */
int32_t rsi_ble_get_long_att_value(uint8_t *dev_addr,
                                   uint16_t handle,
                                   uint16_t offset,
                                   rsi_ble_resp_att_value_t *p_att_vals);

/**
 * @fn         rsi_ble_set_att_value
 */
int32_t rsi_ble_set_att_value(uint8_t *dev_addr, uint16_t handle, uint8_t data_len, uint8_t *p_data);

/**
 * @fn         rsi_ble_set_att_cmd
 */
int32_t rsi_ble_set_att_cmd(uint8_t *dev_addr, uint16_t handle, uint8_t data_len, uint8_t *p_data);

/**
 * @fn         rsi_ble_req_set_long_att_value
 */
int32_t rsi_ble_set_long_att_value(uint8_t *dev_addr,
                                   uint16_t handle,
                                   uint16_t offset,
                                   uint8_t data_len,
                                   uint8_t *p_data);

/**
 * @fn         rsi_ble_prepare_write
 */
int32_t rsi_ble_prepare_write(uint8_t *dev_addr, uint16_t handle, uint16_t offset, uint8_t data_len, uint8_t *p_data);

/**
 * @fn         rsi_ble_execute_write
 */
int32_t rsi_ble_execute_write(uint8_t *dev_addr, uint8_t exe_flag);

/******************************************************
 * *        GATT Server API's Declarations
 * ******************************************************/
/**
 * @fn         rsi_ble_add_service
 */
int32_t rsi_ble_add_service(uuid_t serv_uuid, rsi_ble_resp_add_serv_t *p_resp_serv);

/**
 * @fn         rsi_ble_add_attribute
 */
int32_t rsi_ble_add_attribute(rsi_ble_req_add_att_t *p_attribute);

/**
 * @fn         rsi_ble_set_local_att_value
 */
int32_t rsi_ble_set_local_att_value(uint16_t handle, uint16_t data_len, uint8_t *p_data);

/**
 * @fn         rsi_ble_get_local_att_value
 */
int32_t rsi_ble_get_local_att_value(uint16_t handle, rsi_ble_resp_local_att_value_t *p_resp_local_att_val);

/**
 * @fn         rsi_ble_set_local_irk_value

 * */
int32_t rsi_ble_set_local_irk_value(uint8_t *local_irk);

/*==============================================*/
/**
 * @fn          int32_t rsi_ble_set_ble_tx_power
 */
int32_t rsi_ble_set_ble_tx_power(uint8_t role, uint8_t *remote_dev_address, int8_t tx_power);
/*==============================================*/
/**
 * @fn          int32_t rsi_ble_set_prop_protocol_ble_bandedge_tx_power(uint8_t protocol, int8_t bandedge_tx_power)
 * @brief       Set the Proprietary Protocol and  BLE protocol bandedge tx power
 */

int32_t rsi_ble_set_prop_protocol_ble_bandedge_tx_power(uint8_t protocol, int8_t bandedge_tx_power);

/**
 * @fn         rsi_ble_gatt_read_response
 */
int32_t rsi_ble_gatt_read_response(uint8_t *dev_addr,
                                   uint8_t read_type,
                                   uint16_t handle,
                                   uint16_t offset,
                                   uint16_t length,
                                   uint8_t *p_data);

/**
 * @fn         rsi_ble_smp_pair_request
 */
int32_t rsi_ble_smp_pair_request(uint8_t *remote_dev_address, uint8_t io_capability, uint8_t mitm_req);

/**
 * @fn         rsi_ble_smp_pair_response
 */
int32_t rsi_ble_smp_pair_response(uint8_t *remote_dev_address, uint8_t io_capability, uint8_t mitm_req);

/**
 * @fn         rsi_ble_smp_passkey
 */
int32_t rsi_ble_smp_passkey(uint8_t *remote_dev_address, uint32_t passkey);

/*==============================================*/
/**
 * @fn         rsi_ble_cbfc_connreq 
 */
uint32_t rsi_ble_cbfc_connreq(uint8_t *dev_addr, uint16_t psm);

/*==============================================*/
/**
 * @fn         rsi_ble_cbfc_connresp 
 */
uint32_t rsi_ble_cbfc_connresp(uint8_t *dev_addr, uint16_t lcid, uint8_t result);

/*==============================================*/
/**
 * @fn         rsi_ble_cbfc_data_tx 
 */
uint32_t rsi_ble_cbfc_data_tx(uint8_t *dev_addr, uint16_t lcid, uint16_t len, uint8_t *p_data);

/*==============================================*/
/**
 * @fn         rsi_ble_cbfc_disconnect 
 */
uint32_t rsi_ble_cbfc_disconnect(uint8_t *dev_addr, uint16_t lcid);

/******************************************************
 * *       BLE GAP Callbacks Declarations
 * ******************************************************/
/** @addtogroup BT-LOW-ENERGY8
* @{
*/
/**
 * @typedef    void (*rsi_ble_on_adv_report_event_t)(rsi_ble_event_adv_report_t *rsi_ble_event_adv);
 * @brief      Callback function to be called if advertise report event is received
 * @param[out]  rsi_ble_event_adv contains the advertise report information. Please refer rsi_ble_event_adv_report_s for more info.
 * @return      void
 * @section description
 * This callback function will be called if the advertise event report is received from the module \n
 * This callback has to be registered using rsi_ble_gap_register_callbacks API
 */
typedef void (*rsi_ble_on_adv_report_event_t)(rsi_ble_event_adv_report_t *rsi_ble_event_adv);

/**
 * @typedef   void (*rsi_ble_on_connect_t)(rsi_ble_event_conn_status_t *rsi_ble_event_conn);
 * @brief      Callback function to be called if BLE connection status is received
 * @param[out]  rsi_ble_event_conn contains the BLE connection status. Please refer rsi_ble_event_conn_status_s for more info.
 * @return      void
 * @section description
 * This callback function will be called if the BLE connection status is received from the module. For BLE 4.0 version this callback will be triggered \n
 * This callback has to be registered using rsi_ble_gap_register_callbacks API
 */
typedef void (*rsi_ble_on_connect_t)(rsi_ble_event_conn_status_t *rsi_ble_event_conn);

/**
 * @typedef    void (*rsi_ble_on_enhance_connect_t)(rsi_ble_event_enhance_conn_status_t *rsi_ble_event_enhance_conn);
 * @brief      Callback function to be called if BLE connection status is received
 * @param[out]  rsi_ble_event_conn contains the BLE connection status. Please refer rsi_ble_event_enhance_conn_status_s for more info.
 * @return      void
 * @section description
 * This callback function will be called if the BLE connection status is received from the module. For BLE 4.2 and above version this callback will be triggered  \n
 * This callback has to be registered using rsi_ble_gap_register_callbacks API
 */
typedef void (*rsi_ble_on_enhance_connect_t)(rsi_ble_event_enhance_conn_status_t *rsi_ble_event_enhance_conn);

/**
 * @typedef void (*rsi_ble_on_disconnect_t)(rsi_ble_event_disconnect_t *rsi_ble_event_disconnect, uint16_t reason);
 * @brief      Callback function to be called if disconnect event is received
 * @param[out]  rsi_ble_event_disconnect contains the disconnect status. Please refer rsi_ble_event_disconnect_s for more info.
 * @param[out]  reason contains reason for failure. \n
 * @note        Few reason for failure are given below \n
 *              0x4E13	Remote user terminated connection \n
 *              0x4E14	Remote device terminated connection due to low resources \n
 *              0x4E15	Remote device terminated connection due to power off \n
 *              0x4E3D	Connection terminated due to MIC failure \n
 *              0x4E3E  Connection Failed to be Established \n
 *              0x4E60  Invalid Handle Range
 * @return      void
 * @section description
 * This callback function will be called if the disconnect status event is received from the module \n
 * This callback has to be registered using rsi_ble_gap_register_callbacks API
 */
typedef void (*rsi_ble_on_disconnect_t)(rsi_ble_event_disconnect_t *rsi_ble_event_disconnect, uint16_t reason);

/**
 * @typedef void (*rsi_ble_on_le_ping_payload_timeout_t)(rsi_ble_event_le_ping_time_expired_t *rsi_ble_event_timeout_expired);
 * @brief      Callback function to be called if le ping payload timeout expired event is received
 * @param[out]  rsi_ble_event_disconnect contains the disconnect status. Please refer rsi_ble_event_le_ping_time_expired_s for more info.
 * @return      void
 * @section description
 * This callback function will be called if the le ping time expired event is received from the module \n
 * This callback has to be registered using rsi_ble_gap_register_callbacks API
 */
typedef void (*rsi_ble_on_le_ping_payload_timeout_t)(
  rsi_ble_event_le_ping_time_expired_t *rsi_ble_event_timeout_expired);

/**
 * @typedef    void (*rsi_ble_on_le_ltk_req_event_t)(rsi_bt_event_le_ltk_request_t *rsi_ble_event_le_ltk_request);
 * @brief      Callback function to be called if le ltk request event is received
 * @param[out]  rsi_ble_event_le_ltk_request contains the ltk request info. Please refer  rsi_bt_event_le_ltk_request_s for more info
 * @return      void
 * @section description
 * This callback function will be called if le ltk request event is received from the module \n
 * This callback has to be registered using rsi_ble_smp_register_callbacks API
 */
typedef void (*rsi_ble_on_le_ltk_req_event_t)(rsi_bt_event_le_ltk_request_t *rsi_ble_event_le_ltk_request);

/**
 * @typedef    void (*rsi_ble_on_le_security_keys_t)(rsi_bt_event_le_security_keys_t *rsi_ble_event_le_security_keys);
 * @brief      Callback function to be called if le security keys event is received
 * @param[out]  rsi_bt_event_le_security_keys_t contains security keys. Please refer rsi_bt_event_le_security_keys_s for more info
 * @return      void
 * @section description
 * This callback function will be called if le security keys event is received from the module \n
 * This callback has to be registered using rsi_ble_smp_register_callbacks API
 */
typedef void (*rsi_ble_on_le_security_keys_t)(rsi_bt_event_le_security_keys_t *rsi_ble_event_le_security_keys);
/** @} */
/******************************************************
 * *       BLE SMP EVENT Callbacks Declarations
 * ******************************************************/
/** @addtogroup BT-LOW-ENERGY8
* @{
*/
/**
 * @typedef    void (*rsi_ble_on_smp_request_t)(rsi_bt_event_smp_req_t *remote_dev_address);
 * @brief      Callback function to be called if smp request is received in Master mode
 * @param[out] resp_status contains the response status (Success or Error code)
 * @param[out] remote_dev_address contains the smp requested device address. Please refer rsi_bt_event_smp_req_s for more info.
 * @return     void
 * @section description
 * This callback function will be called if the  smp request is received from the remote device \n
 * This callback has to be registered using rsi_ble_smp_register_callbacks API
 */
typedef void (*rsi_ble_on_smp_request_t)(rsi_bt_event_smp_req_t *remote_dev_address);

/*==============================================*/
/**
 * @typedef    void (*rsi_ble_on_smp_response_t)(rsi_bt_event_smp_resp_t *remote_dev_address);
 * @brief      Callback function to be called if smp request is received in slave mode
 * @param[out] remote_dev_address contains the smp resp information. please refer rsi_bt_event_smp_resp_s for more info
 * @return     void
 * @section description
 * This callback function will be called if the  smp request is received from the remote device \n
 * This callback has to be registered using rsi_ble_smp_register_callbacks API
 */
typedef void (*rsi_ble_on_smp_response_t)(rsi_bt_event_smp_resp_t *remote_dev_address);

/*==============================================*/
/**
 * @typedef    void (*rsi_ble_on_smp_passkey_t)(rsi_bt_event_smp_passkey_t *remote_dev_address);
 * @brief      Callback function to be called if smp passkey event is received from module
 * @param[out] resp_status, contains the response status (Success or Error code)
 * @param[out] remote_dev_address contains the remote device address. please refer rsi_bt_event_smp_passkey_s for more info
 * @return     void
 * @section description
 * This callback function will be called if the  smp passkey is received from the module \n
 * This callback has to be registered using rsi_ble_smp_register_callbacks API
 */
typedef void (*rsi_ble_on_smp_passkey_t)(rsi_bt_event_smp_passkey_t *remote_dev_address);

/*==============================================*/
/**
 * @typedef    void (*rsi_ble_on_smp_passkey_display_t)(rsi_bt_event_smp_passkey_display_t *smp_passkey_display);
 * @brief      Callback function to be called if smp passkey event is received from module
 * @param[out] resp_status contains the response status (Success or Error code)
 * @param[out] smp_passkey_display contains the smp passkey display information. Please refer rsi_bt_event_smp_passkey_display_s for more info.
 * @return     void
 * @section description
 * This callback function will be called if the  smp passkey display is received from the module \n
 * This callback has to be registered using rsi_ble_smp_register_callbacks API
 */
typedef void (*rsi_ble_on_smp_passkey_display_t)(rsi_bt_event_smp_passkey_display_t *smp_passkey_display);

/*==============================================*/
/**
 * @typedef    void (*rsi_ble_on_smp_failed_t)(uint16_t resp_status, rsi_bt_event_smp_failed_t *remote_dev_address);
 * @brief      Callback function to be called if smp failed event is received from module
 * @param[out] resp_status contains the response status (Success or Error code) \n
 * @note       Error codes for SMP FAILED are given below \n
 *              0x4B01	SMP Passkey entry failed  \n
 *              0x4B02	SMP OOB not available \n
 *              0x4B03	SMP Authentication Requirements \n
 *              0x4B04	SMP confirm value failed \n
 *              0x4B05	SMP Pairing not supported \n
 *              0x4B06	SMP Encryption key size insufficient \n
 *              0x4B07	SMP command not supported \n
 *              0x4B08	SMP Unspecified Reason \n
 *              0x4B09	SMP repeated attempts \n
 *              0x4B0C	SMP Numeric Comparison Failed \n
 *              0x4B0B DHKEY Check Failed
 * @param[out] remote_dev_address contains the remote device address. Please refer rsi_bt_event_smp_failed_s for more info.
 * @return     void
 * @section description
 * This callback function will be called if the  smp process is failed with remote device \n
 * This callback has to be registered using rsi_ble_smp_register_callbacks API
 */
typedef void (*rsi_ble_on_smp_failed_t)(uint16_t resp_status, rsi_bt_event_smp_failed_t *remote_dev_address);

/*==============================================*/
/**
 * @typedef   void (*rsi_ble_on_sc_method_t)(rsi_bt_event_sc_method_t *scmethod);
 * @brief      Callback function to be called if security method event is received from module 
 * @param[out] scmethod contains Security Method 1 means Just works or 2 means Passkey. Please refer rsi_bt_event_sc_method_s for more info.
 * @return     void
 * @section description
 * This callback function will be called if the  SC method is done with remote device \n
 * This callback has to be registered using rsi_ble_smp_register_callbacks API
 */
typedef void (*rsi_ble_on_sc_method_t)(rsi_bt_event_sc_method_t *scmethod);

/*==============================================*/
/**
 * @typedef    void (*rsi_ble_on_encrypt_started_t)(uint16_t resp_status, rsi_bt_event_encryption_enabled_t *enc_enabled);
 * @brief      Callback function to be called if encrypted event is received from module
 * @param[out] resp_status contains the response status (Success or Error code)
 * @param[out] enc_enabled contains encryption information. Please refer rsi_bt_event_encryption_enabled_s for more info. 
 * @return     void
 * @section description
 * This callback function will be called if the  encryption process is started with remote device \n
 * This callback has to be registered using rsi_ble_smp_register_callbacks API
 */
typedef void (*rsi_ble_on_encrypt_started_t)(uint16_t resp_status, rsi_bt_event_encryption_enabled_t *enc_enabled);

/*==============================================*/
/**
 * @typedef void (*rsi_ble_on_sc_passkey_t)(rsi_bt_event_sc_passkey_t *sc_passkey);
 * @brief      Callback function to be called if BLE Secure connection passkey event received from module
 * @param[out] sc_passkey contains LE SC Passkey information. Please refer rsi_bt_event_encryption_enabled_s for more info.
 * @return     void
 * @section description
 * This callback function will be called if the  BLE Secure connection passkey event received \n
 * This callback has to be registered using rsi_ble_smp_register_callbacks API
 */
typedef void (*rsi_ble_on_sc_passkey_t)(rsi_bt_event_sc_passkey_t *sc_passkey);

/**
 * @typedef    void (*rsi_ble_on_phy_update_complete_t)(rsi_ble_event_phy_update_t *rsi_ble_event_phy_update_complete);
 * @brief      Callback function to be called when phy update complete event is received
 * @param[out] rsi_ble_event_phy_update_complete  contains the controller support phy information. Please refer rsi_ble_event_phy_update_s for more info.
 * @return      void
 * @section description
 * This callback function will be called when phy update complete event is received \n
 * This callback has to be registered using rsi_ble_gap_register_callbacks API
 */
typedef void (*rsi_ble_on_phy_update_complete_t)(rsi_ble_event_phy_update_t *rsi_ble_event_phy_update_complete);

/**
 * @typedef void (*rsi_ble_on_conn_update_complete_t)(rsi_ble_event_conn_update_t *rsi_ble_event_conn_update_complete,
                                                  uint16_t resp_status);
 * @brief      Callback function to be called when conn update complete event is received
 * @param[out] rsi_ble_event_conn_update_complete contains the controller support conn information. Please refer rsi_ble_event_conn_update_s for more info.
 * @param[out] resp_status contains the response status (Success or Error code)
 * @return      void
 * @section description
 * This callback function will be called when conn update complete event is received \n
 * This callback has to be registered using rsi_ble_gap_register_callbacks API
 */
typedef void (*rsi_ble_on_conn_update_complete_t)(rsi_ble_event_conn_update_t *rsi_ble_event_conn_update_complete,
                                                  uint16_t resp_status);
/**
 * @typedef void (*rsi_ble_on_remote_conn_params_request_t)(
 *                              rsi_ble_event_remote_conn_param_req_t *rsi_ble_event_remote_conn_param,
 *                              uint16_t resp_status);
 * @brief      Callback function to be called if remote conn params request is received.
 * @param[out] resp_status contains the response status (Success or Error code)
 * @param[out] rsi_ble_event_remote_features contains the remote device supported features. Please refer rsi_ble_event_remote_features_s for more info.
 * @return      void
 * @section description
 * This callback function will be called when remote conn params request is received \n
 * This callback has to be registered using rsi_ble_gap_register_callbacks API
 */
typedef void (*rsi_ble_on_remote_conn_params_request_t)(
  rsi_ble_event_remote_conn_param_req_t *rsi_ble_event_remote_conn_param,
  uint16_t resp_status);

/**
 * @typedef void (*rsi_ble_on_remote_features_t)(rsi_ble_event_remote_features_t *rsi_ble_event_remote_features);
 * @brief      Callback function to peer device supported features.
 * @param[out] rsi_ble_event_remote_features contains the remote device supported features. Please refer rsi_ble_event_remote_features_s for more info.
 * @return     void
 * @section    description
 * This callback function will be called when conn update complete event is received \n
 * This callback has to be registered using rsi_ble_gap_extended_register_callbacks API
 */
typedef void (*rsi_ble_on_remote_features_t)(rsi_ble_event_remote_features_t *rsi_ble_event_remote_features);

/**
 * @typedef    void (*rsi_ble_on_le_more_data_req_t)(rsi_ble_event_le_dev_buf_ind_t *rsi_ble_more_data_evt);
 * @brief      Callback function to le more data request
 * @param[out] rsi_ble_more_data_evt contains the LE Device Buffer Indication information. Please refer rsi_ble_event_le_dev_buf_ind_s for more info.
 * @return     void
 * @section    description
 * This callback function will be called when le more data event is received \n
 * This callback has to be registered using rsi_ble_gap_extended_register_callbacks API
 */
typedef void (*rsi_ble_on_le_more_data_req_t)(rsi_ble_event_le_dev_buf_ind_t *rsi_ble_more_data_evt);

/*==============================================*/
/**
 * @typedef    void (*rsi_ble_on_data_length_update_t)(rsi_ble_event_data_length_update_t *remote_dev_address);
 * @brief      This event is raised when data length is update in controller
 * @param[out] remote_dev_address contains the controller support tx and rx length information. Please refer rsi_ble_event_data_length_update_s for more info.
 * @return     void
 * @section    description
 * This callback function will be called when data length update complete event is received \n
 * This callback has to be registered using rsi_ble_gap_register_callbacks API
 */
typedef void (*rsi_ble_on_data_length_update_t)(rsi_ble_event_data_length_update_t *remote_dev_address);

/**
 * @typedef    void (*rsi_ble_on_directed_adv_report_event_t)(rsi_ble_event_directedadv_report_t *rsi_ble_event_directed);
 * @brief      Callback function to be called if directed advertise report event is received
 * @param[in]  rsi_ble_event_directed, contains the advertise report information
 * @return     void
 * @section    description
 * This callback function will be called if the advertise event report is received from the module \n
 * This callback has to be registered using rsi_ble_gap_register_callbacks API
 */
typedef void (*rsi_ble_on_directed_adv_report_event_t)(rsi_ble_event_directedadv_report_t *rsi_ble_event_directed);
/** @} */
/*==============================================*/
/**
 * @fn         rsi_ble_gap_register_callbacks
 */
void rsi_ble_gap_register_callbacks(rsi_ble_on_adv_report_event_t ble_on_adv_report_event,
                                    rsi_ble_on_connect_t ble_on_conn_status_event,
                                    rsi_ble_on_disconnect_t ble_on_disconnect_event,
                                    rsi_ble_on_le_ping_payload_timeout_t timeout_expired_event,
                                    rsi_ble_on_phy_update_complete_t ble_on_phy_update_complete_event,
                                    rsi_ble_on_data_length_update_t ble_on_data_length_update_complete_event,
                                    rsi_ble_on_enhance_connect_t ble_on_enhance_conn_status_event,
                                    rsi_ble_on_directed_adv_report_event_t ble_on_directed_adv_report_event,
                                    rsi_ble_on_conn_update_complete_t ble_on_conn_update_complete_event,
                                    rsi_ble_on_remote_conn_params_request_t ble_on_remote_conn_params_request_event);

/*==============================================*/
/**
 * @fn         rsi_ble_smp_register_callbacks
 */
void rsi_ble_smp_register_callbacks(rsi_ble_on_smp_request_t ble_on_smp_request_event,
                                    rsi_ble_on_smp_response_t ble_on_smp_response_event,
                                    rsi_ble_on_smp_passkey_t ble_on_smp_passkey_event,
                                    rsi_ble_on_smp_failed_t ble_on_smp_fail_event,
                                    rsi_ble_on_encrypt_started_t rsi_ble_on_encrypt_started_event,
                                    rsi_ble_on_smp_passkey_display_t ble_on_smp_passkey_display_event,
                                    rsi_ble_on_sc_passkey_t ble_sc_passkey_event,
                                    rsi_ble_on_le_ltk_req_event_t ble_on_le_ltk_req_event,
                                    rsi_ble_on_le_security_keys_t ble_on_le_security_keys_event,
                                    rsi_ble_on_smp_response_t ble_on_cli_smp_response_event,
                                    rsi_ble_on_sc_method_t ble_on_sc_method_event);

/******************************************************
 * *       BLE GATT Response Callbacks Declarations
 * ******************************************************/
/** @addtogroup BT-LOW-ENERGY8
* @{
*/
/**
 * @typedef    void (*rsi_ble_on_gatt_error_resp_t)(uint16_t event_status, rsi_ble_event_error_resp_t *rsi_ble_gatt_error);
 * @brief      Callback function to be called if gatt error event is received
 * @param[out]  event_status contains the error response  \n
 *              Non-Zero Value	-	Failure \n
 * @note        Attribute protocol error codes \n 
 *              0x4A01  -  Invalid Handle \n
 *              0x4A06  -  Request not supported \n
 *              0x4A0A  -  Attribute not found \n
 *              0x4A05  -  Insufficient authentication \n
 *              0x4A08  -  Insufficient authorization \n
 *              0x4A0C  -  Insufficient encryption key size \n
 *              0x4A0F  -  Insufficient encryption \n
 * 				0x4A02  -  Read not permitted \n 
 *              0x4A03  -  Write not permitted \n
 *              0x4A07  -  Invalid offset \n
 *              0x4A0B  -  Attribute not Long \n
 * @param[out]  rsi_ble_gatt_error contains the gatt error information. Please refer rsi_ble_event_error_resp_s for more info
 * @return      void
 * @section description
 * This callback function will be called if the  gatt error event is received from the module \n
 * This callback has to be registered using rsi_ble_gatt_register_callbacks API
 */
typedef void (*rsi_ble_on_gatt_error_resp_t)(uint16_t event_status, rsi_ble_event_error_resp_t *rsi_ble_gatt_error);
/**
 * @typedef void (*rsi_ble_on_gatt_desc_val_event_t)(uint16_t event_status,
                                                 rsi_ble_event_gatt_desc_t *rsi_ble_gatt_desc_val);
 * @brief      Callback function to be called if attribute descriptors event is received
 * @param[out]  event_status contains the response status \n 
 *              0 - Success \n 
 *              Non-Zero Value	-	Failure \n
 * @param[out]  rsi_ble_gatt_desc_val contains the profiles list event information. Please refer rsi_ble_event_gatt_desc_s for more info
 * @return      void
 * @section description
 * This callback function will be called if the attribute descriptors event is received from the module \n
 * This callback has to be registered using rsi_ble_gatt_register_callbacks API
 */
typedef void (*rsi_ble_on_gatt_desc_val_event_t)(uint16_t event_status,
                                                 rsi_ble_event_gatt_desc_t *rsi_ble_gatt_desc_val);

/**
 * @typedef void (*rsi_ble_on_event_profiles_list_t)(uint16_t event_status,
                                                 rsi_ble_event_profiles_list_t *rsi_ble_event_profiles);
 * @brief      Callback function to be called if profiles list event is received
 * @param[out]  event_status contains the response status \n 
 *              0 - Success \n 
 *              Non-Zero Value	-	Failure \n
 * @param[out]  rsi_ble_event_profiles contains the profiles list event information. Please refer rsi_ble_event_profiles_list_s for more info
 * @return      void
 * @section description
 * This callback function will be called if the  profiles list response is received from the module \n
 * This callback has to be registered using rsi_ble_gatt_register_callbacks API
 */
typedef void (*rsi_ble_on_event_profiles_list_t)(uint16_t event_status,
                                                 rsi_ble_event_profiles_list_t *rsi_ble_event_profiles);

/**
 * @typedef void (*rsi_ble_on_event_profile_by_uuid_t)(uint16_t event_status,
                                                   rsi_ble_event_profile_by_uuid_t *rsi_ble_event_profile);
 * @brief      Callback function to be called if profile event is received
 * @param[out]  event_status contains the response status \n 
 *              0 - Success \n 
 *              Non-Zero Value	-	Failure \n
 * @param[out]  rsi_ble_event_profile contains the profile response information. Please refer rsi_ble_event_profile_by_uuid_s for more info.
 * @return      void
 * @section description
 * This callback function will be called if the  profile response is received from the module \n
 * This callback has to be registered using rsi_ble_gatt_register_callbacks API
 */
typedef void (*rsi_ble_on_event_profile_by_uuid_t)(uint16_t event_status,
                                                   rsi_ble_event_profile_by_uuid_t *rsi_ble_event_profile);
/**
 * @typedef void (*rsi_ble_on_event_read_by_char_services_t)(uint16_t event_status,
                                                         rsi_ble_event_read_by_type1_t *rsi_ble_event_read_type1);
 * @brief      Callback function to be called if characteristic services list event is received
 * @param[out]  event_status contains the response status \n 
 *              0 - Success \n 
 *              Non-Zero Value	-	Failure \n
 * @param[out]  rsi_ble_event_read_type1 contains the char services event information. Please refer rsi_ble_event_read_by_type1_s for more info.
 * @return      void
 * @section description
 * This callback function will be called if the characteristic services list response is received from the module \n 
 * This callback has to be registered using rsi_ble_gatt_register_callbacks API
 */
typedef void (*rsi_ble_on_event_read_by_char_services_t)(uint16_t event_status,
                                                         rsi_ble_event_read_by_type1_t *rsi_ble_event_read_type1);

/**
 * @typedef void (*rsi_ble_on_event_read_by_inc_services_t)(uint16_t event_status,
                                                        rsi_ble_event_read_by_type2_t *rsi_ble_event_read_type2);
 * @brief      Callback function to be called if include services list event is received
 * @param[out]  event_status contains the response status \n 
 *              0 - Success \n 
 *              Non-Zero Value	-	Failure \n
 * @param[out]  rsi_ble_event_read_type2 contains the inc services information. Please refer rsi_ble_event_read_by_type2_s for more info.
 * @return      void
 * @section description
 * This callback function will be called if the  include services list response is received from the module \n
 * This callback has to be registered using rsi_ble_gatt_register_callbacks API
 */
typedef void (*rsi_ble_on_event_read_by_inc_services_t)(uint16_t event_status,
                                                        rsi_ble_event_read_by_type2_t *rsi_ble_event_read_type2);
/**
 * @typedef void (*rsi_ble_on_event_read_att_value_t)(uint16_t event_status,
                                                  rsi_ble_event_read_by_type3_t *rsi_ble_event_read_type3);
 * @brief      Callback function to be called if attribute value event is received
 * @param[out]  event_status contains the response status \n 
 *              0 - Success \n 
 *              Non-Zero Value	-	Failure \n
 * @param[out]  rsi_ble_event_read_type3 contains the char services event information. Please refer rsi_ble_event_read_by_type3_s for more info.
 * @return      void
 * @section description
 * This callback function will be called if the  attribute value response is received from the module \n
 * This callback has to be registered using rsi_ble_gatt_register_callbacks API
 */
typedef void (*rsi_ble_on_event_read_att_value_t)(uint16_t event_status,
                                                  rsi_ble_event_read_by_type3_t *rsi_ble_event_read_type3);

/**
 * @typedef void (*rsi_ble_on_event_read_resp_t)(uint16_t event_status, rsi_ble_event_att_value_t *rsi_ble_event_att_val);
 * @brief      Callback function to be called if attribute value event is received
 * @param[out]  event_status contains the response status \n 
 *              0 - Success \n 
 *              Non-Zero Value	-	Failure \n
 * @param[out]  rsi_ble_event_att_val contains the profile response information. Please refer rsi_ble_event_att_value_s for more info.
 * @return      void
 * @section description
 * This callback function will be called if the  attribute value is received from the module \n 
 * This callback has to be registered using rsi_ble_gatt_register_callbacks API
 */
typedef void (*rsi_ble_on_event_read_resp_t)(uint16_t event_status, rsi_ble_event_att_value_t *rsi_ble_event_att_val);

/**
 * @typedef    void (*rsi_ble_on_event_write_resp_t)(uint16_t event_status, rsi_ble_set_att_resp_t *rsi_ble_event_set_att_rsp);
 * @brief      Callback function to be called if gatt write event is received
 * @param[out]  event_status contains the response status \n 
 *              0 - Success \n 
 *              Non-Zero Value	-	Failure \n
 * @param[out]  rsi_ble_event_set_att_rsp contains the profile response information. Please refer rsi_ble_set_att_resp_t for more info.
 * @return      void
 * @section description
 * This callback function will be called if the  gatt write response is received from the module \n
 * This callback has to be registered using rsi_ble_gatt_register_callbacks API
 */
typedef void (*rsi_ble_on_event_write_resp_t)(uint16_t event_status, rsi_ble_set_att_resp_t *rsi_ble_event_set_att_rsp);
/**
 * @typedef void (*rsi_ble_on_event_indicate_confirmation_t)(uint16_t event_status,
                                                         rsi_ble_set_att_resp_t *rsi_ble_event_set_att_rsp);
 * @brief      Callback function to be called if indication confirmation event is received
 * @param[out]  event_status contains the response status \n 
 *              0 - Success \n 
 *              Non-Zero Value	-	Failure \n
 * @param[out]  rsi_ble_event_set_att_rsp contains the profile response information. Please refer rsi_ble_set_att_resp_s for more info.
 * @return      void
 * @section description
 * This callback function will be called if the  indication confirmation response is received from the module \n
 * This callback has to be registered using rsi_ble_gatt_register_callbacks API
 */
typedef void (*rsi_ble_on_event_indicate_confirmation_t)(uint16_t event_status,
                                                         rsi_ble_set_att_resp_t *rsi_ble_event_set_att_rsp);
/**
 * @typedef void (*rsi_ble_on_event_prepare_write_resp_t)(uint16_t event_status,
                                                      rsi_ble_prepare_write_resp_t *rsi_ble_event_prepare_write);
 * @brief      Callback function to be called if GATT prepare event is received
 * @param[out]  event_status contains the response status \n 
 *              0 - Success \n 
 *              Non-Zero Value	-	Failure \n
 * @param[out]  rsi_ble_event_prepare_write contains the char services event information. Please refer rsi_ble_prepare_write_resp_s for more info.
 * @return      void
 * @section description
 * This callback function will be called if the  GATT prepare response is received from the module \n
 * This callback has to be registered using rsi_ble_gatt_register_callbacks API
 */
typedef void (*rsi_ble_on_event_prepare_write_resp_t)(uint16_t event_status,
                                                      rsi_ble_prepare_write_resp_t *rsi_ble_event_prepare_write);

/**
 * @typedef void (*rsi_ble_on_profiles_list_resp_t)(uint16_t resp_status,
                                                rsi_ble_resp_profiles_list_t *rsi_ble_resp_profiles);
 * @brief      Callback function to be called if profiles list response is received
 * @param[out]  resp_status contains the response status \n
 *              0 - Success \n 
 *              Non-Zero Value	-	Failure \n 
 * @note        Attribute protocol error codes \n
 *              0x4A01  -  Invalid Handle \n
 *              0x4A0A  -  Attribute not found \n
 * @param[out]  rsi_ble_resp_profiles contains the profiles list response information. Please refer rsi_ble_resp_profiles_list_s for more info.
 * @return      void
 * @section description
 * This callback function will be called if the  profiles list response is received from the module \n
 * This callback has to be registered using rsi_ble_gatt_register_callbacks API
 */
typedef void (*rsi_ble_on_profiles_list_resp_t)(uint16_t resp_status,
                                                rsi_ble_resp_profiles_list_t *rsi_ble_resp_profiles);

/**
 * @typedef void (*rsi_ble_on_profile_resp_t)(uint16_t resp_status, profile_descriptors_t *rsi_ble_resp_profile);
 * @brief      Callback function to be called if profile response is received
 * @param[out]  resp_status contains the response status \n
 *              0 - Success \n 
 *              Non-Zero Value	-	Failure \n
 * @note        Attribute protocol error codes \n 
 *              0x4A01  -  Invalid Handle \n
 *              0x4A06  -  Request not supported \n
 *              0x4A0A  -  Attribute not found \n
 * @param[out]  rsi_ble_resp_profile contains the profile response information. Please refer profile_descriptors_s for more info
 * @return      void
 * @section description
 * This callback function will be called if the  profile response is received from the module \n
 * This callback has to be registered using rsi_ble_gatt_register_callbacks API
 */
typedef void (*rsi_ble_on_profile_resp_t)(uint16_t resp_status, profile_descriptors_t *rsi_ble_resp_profile);

/**
 * @typedef void (*rsi_ble_on_char_services_resp_t)(uint16_t resp_status,
                                                rsi_ble_resp_char_services_t *rsi_ble_resp_char_serv);
 * @brief      Callback function to be called if service characteristics response is received
 * @param[out]  resp_status contains the response status \n 
 *              0 - Success \n 
 *              Non-Zero Value	-	Failure \n
 * @note        Attribute protocol error codes \n 
 *              0x4A01  -  Invalid Handle \n
 *              0x4A06  -  Request not supported \n
 *              0x4A0A  -  Attribute not found \n
 *              0x4A05  -  Insufficient authentication \n
 *              0x4A08  -  Insufficient authorization \n
 *              0x4A0C  -  Insufficient encryption key size \n
 *              0x4A0F  -  Insufficient encryption \n
 * 				0x4A02  -  Read not permitted \n 
 * @param[out]  rsi_ble_resp_char_serv contains the service characteristics response information. Please refer rsi_ble_resp_char_services_s for more info
 * @return      void
 * @section description
 * This callback function will be called if the  service characteristics response is received from the module \n
 * This callback has to be registered using rsi_ble_gatt_register_callbacks API
 */
typedef void (*rsi_ble_on_char_services_resp_t)(uint16_t resp_status,
                                                rsi_ble_resp_char_services_t *rsi_ble_resp_char_serv);

/**
 * @typedef void (*rsi_ble_on_inc_services_resp_t)(uint16_t resp_status,
                                               rsi_ble_resp_inc_services_t *rsi_ble_resp_inc_serv);
 * @brief      Callback function to be called if include services response is received
 * @param[out]  resp_status contains the response status \n 
 *              0 - Success \n 
 *              Non-Zero Value	-	Failure \n
 * @note        Attribute protocol error codes \n 
 *              0x4A01  -  Invalid Handle \n
 *              0x4A06  -  Request not supported \n
 *              0x4A0A  -  Attribute not found \n
 *              0x4A05  -  Insufficient authentication \n
 *              0x4A08  -  Insufficient authorization \n
 *              0x4A0C  -  Insufficient encryption key size \n
 *              0x4A0F  -  Insufficient encryption \n
 * 				0x4A02  -  Read not permitted \n 
 * @param[out]  rsi_ble_resp_inc_serv contains the include services response information. Please refer rsi_ble_resp_inc_services_s for more info
 * @return      void
 * @section description
 * This callback function will be called if the  include service response is received from the module \n
 * This callback has to be registered using rsi_ble_gatt_register_callbacks API
 */
typedef void (*rsi_ble_on_inc_services_resp_t)(uint16_t resp_status,
                                               rsi_ble_resp_inc_services_t *rsi_ble_resp_inc_serv);

/**
 * @typedef     void (*rsi_ble_on_att_desc_resp_t)(uint16_t resp_status, rsi_ble_resp_att_descs_t *rsi_ble_resp_att_desc);
 * @brief      Callback function to be called if attribute descriptors response is received
 * @param[out]  resp_status contains the response status \n 
 *              0 - Success \n 
 *              Non-Zero Value	-	Failure \n
 * @note        Attribute protocol error codes \n 
 *              0x4A01  -  Invalid Handle \n
 *              0x4A0A  -  Attribute not found \n
 *              0x4A05  -  Insufficient authentication \n
 *              0x4A08  -  Insufficient authorization \n
 *              0x4A0C  -  Insufficient encryption key size \n
 *              0x4A0F  -  Insufficient encryption \n
 * 				0x4A02  -  Read not permitted \n 
 * @param[out]  rsi_ble_resp_att_desc contains the attribute descriptors response information. Please refer rsi_ble_resp_att_descs_s for more info 
 * @return      void
 * @section description
 * This callback function will be called if the attribute descriptors  response is received from the module \n
 * This callback has to be registered using rsi_ble_gatt_register_callbacks API
 */
typedef void (*rsi_ble_on_att_desc_resp_t)(uint16_t resp_status, rsi_ble_resp_att_descs_t *rsi_ble_resp_att_desc);

/**
 * @typedef void (*rsi_ble_on_read_resp_t)(uint16_t resp_status,
                                       uint16_t resp_id,
                                       rsi_ble_resp_att_value_t *rsi_ble_resp_att_val);
 * @brief      Callback function to be called upon receiving the attribute value
 * @param[out]  resp_status contains the response status \n 
 *              0 - Success \n 
 *              Non-Zero Value	-	Failure \n
 * @note        Attribute protocol error codes \n 
 *              0x4A01  -  Invalid Handle \n
 *              0x4A0A  -  Attribute not found \n
 *              0x4A05  -  Insufficient authentication \n
 *              0x4A08  -  Insufficient authorization \n
 *              0x4A0C  -  Insufficient encryption key size \n
 *              0x4A0F  -  Insufficient encryption \n
 * 				0x4A02  -  Read not permitted \n 
 *              0x4A06  -  Request not supported \n
 *              0x4A07  -  Invalid offset \n
 *              0x4A0B  -  Attribute not Long \n
 * @param[out]  resp_id contains the response id because of which, this callback is called
 *                      response ids: (RSI_BLE_RSP_READ_VAL, RSI_BLE_RSP_READ_BY_UUID, RSI_BLE_RSP_LONG_READ, RSI_BLE_RSP_MULTIPLE_READ)
 * @param[out]  rsi_ble_resp_att_val contains the attribute value. Please refer rsi_ble_resp_att_value_s for more info
 * @return      void
 * @section description
 * This callback function will be called upon receiving the attribute value \n
 * This callback has to be registered using rsi_ble_gatt_register_callbacks API
 */
typedef void (*rsi_ble_on_read_resp_t)(uint16_t resp_status,
                                       uint16_t resp_id,
                                       rsi_ble_resp_att_value_t *rsi_ble_resp_att_val);

/**
 * @typedef void (*rsi_ble_on_write_resp_t)(uint16_t resp_status, uint16_t resp_id);
 * @brief      Callback function to be called if the attribute set/prepare/execute action is completed
 * @param[out]  resp_status contains the response status \n 
 *              0 - Success \n 
 *              Non-Zero Value	-	Failure \n
 * @note        Attribute protocol error codes \n 
 *              0x4A01  -  Invalid Handle \n
 *              0x4A0A  -  Attribute not found \n
 *              0x4A05  -  Insufficient authentication \n
 *              0x4A08  -  Insufficient authorization \n
 *              0x4A0C  -  Insufficient encryption key size \n
 *              0x4A0F  -  Insufficient encryption \n
 * 				0x4A03  -  Write not permitted \n
 *              0x4A07  -  Invalid offset  \n
 *              0x4A0D  -  Invalid attribute value length \n
 * @param[out]  resp_id contains the response id because of which, this callback is called
 *                      response ids: (RSI_BLE_RSP_WRITE, RSI_BLE_RSP_WRITE_NO_ACK, RSI_BLE_RSP_LONG_WRITE, RSI_BLE_RSP_EXECUTE_WRITE)
 * @return      void
 * @section description
 * This callback function will be called if the attribute set/prepare/execute action is completed \n
 * This callback has to be registered using rsi_ble_gatt_register_callbacks API
 */
typedef void (*rsi_ble_on_write_resp_t)(uint16_t resp_status, uint16_t resp_id);
/** @} */
/******************************************************
 * *       BLE GATT Event Callbacks Declarations
 * ******************************************************/
/** @addtogroup BT-LOW-ENERGY8
* @{
*/
/**
 * @typedef void (*rsi_ble_on_gatt_write_event_t)(uint16_t event_id, rsi_ble_event_write_t *rsi_ble_write);
 * @brief      Callback function to be called if the GATT write/notify/indicate events are received
 * @param[out]  event_id contains the response status \n 
 *              0 - Success \n 
 *              Non-Zero Value	-	Failure \n
 * @param[out]  rsi_ble_write  contains the GATT event information. Please refer rsi_ble_event_write_s for more info
 * @return      void
 * @section description
 * This callback function will be called if the GATT write/notify/indicate events are received \n
 * This callback has to be registered using rsi_ble_gatt_register_callbacks API
 */
typedef void (*rsi_ble_on_gatt_write_event_t)(uint16_t event_id, rsi_ble_event_write_t *rsi_ble_write);

/**
 * @typedef void (*rsi_ble_on_gatt_prepare_write_event_t)(uint16_t event_id, rsi_ble_event_prepare_write_t *rsi_ble_write);
 * @brief      Callback function to be called if the GATT prepare events are received
 * @param[out]  event_id contains the response status \n 
 *              0 - Success \n 
 *              Non-Zero Value	-	Failure \n
 * @param[out]  rsi_ble_write  contains the GATT prepare event information. Please refer rsi_ble_event_prepare_write_s for more info
 * @return      void
 * @section description
 * This callback function will be called if the GATT prepare event is received \n
 * This callback has to be registered using rsi_ble_gatt_register_callbacks API
 */
typedef void (*rsi_ble_on_gatt_prepare_write_event_t)(uint16_t event_id, rsi_ble_event_prepare_write_t *rsi_ble_write);

/**
 * @typedef void (*rsi_ble_on_execute_write_event_t)(uint16_t event_id, rsi_ble_execute_write_t *rsi_ble_execute_write);
 * @brief      Callback function to be called if the GATT execute events are received
 * @param[out]  event_id contains the response status \n 
 *              0 - Success \n 
 *              Non-Zero Value	-	Failure \n
 * @param[out]  rsi_ble_write  contains the GATT event information. Please refer rsi_ble_execute_write_s for more info.
 * @return      void
 * @section description
 * This callback function will be called if the GATT execute event is received \n
 * This callback has to be registered using rsi_ble_gatt_register_callbacks API
 */
typedef void (*rsi_ble_on_execute_write_event_t)(uint16_t event_id, rsi_ble_execute_write_t *rsi_ble_execute_write);

/**
 * @typedef    void (*rsi_ble_on_read_req_event_t)(uint16_t event_id, rsi_ble_read_req_t *rsi_ble_read_req);
 * @brief      Callback function to be called if the GATT read request events are received
 * @param[out]  event_id contains the response status \n 
 *              0 - Success \n 
 *              Non-Zero Value	-	Failure \n
 * @param[out]  rsi_ble_write contains the GATT event information. Please refer rsi_ble_read_req_s for more info.
 * @return      void
 * @section description
 * This callback function will be called if the GATT read request event is received \n
 * This callback has to be registered using rsi_ble_gatt_register_callbacks API
 */
typedef void (*rsi_ble_on_read_req_event_t)(uint16_t event_id, rsi_ble_read_req_t *rsi_ble_read_req);

/**
 * @typedef void (*rsi_ble_on_mtu_event_t)(rsi_ble_event_mtu_t *rsi_ble_event_mtu);
 * @brief      Callback function to be called if MTU size request is received.
 * @param[out]  rsi_ble_event_mtu  contains the MTU size information. Please refer rsi_ble_event_mtu_s for more info.
 * @return      void
 * @section description
 * This callback function will be called when connected to indicate MTU size \n
 * This callback has to be registered using rsi_ble_gatt_register_callbacks API
 */
typedef void (*rsi_ble_on_mtu_event_t)(rsi_ble_event_mtu_t *rsi_ble_event_mtu);

/**
 * @typedef    void (*rsi_ble_on_mtu_exchange_info_t)(rsi_ble_event_mtu_exchange_information_t *rsi_ble_event_mtu_exchange_info);
 * @brief      Callback function to indicate MTU size and who initated MTU Exchange Request
 * @param[out]  rsi_ble_event_mtu_exchange_info contains the MTU exchange information. Please refer rsi_ble_event_mtu_exchange_information_s for more info.
 * @return      void
 * @section description
 * This callback function will be called when connected, this event will contain MTU Exchange Information \n
 * This callback has to be registered using rsi_ble_gatt_extended_register_callbacks API
 */
typedef void (*rsi_ble_on_mtu_exchange_info_t)(
  rsi_ble_event_mtu_exchange_information_t *rsi_ble_event_mtu_exchange_info);

/** @} */
/******************************************************
 * * BLE GATT Callbacks register function Declarations
 * ******************************************************/

/*==============================================*/
/**
 * @fn         rsi_ble_gatt_register_callbacks
 */
void rsi_ble_gatt_register_callbacks(rsi_ble_on_profiles_list_resp_t ble_on_profiles_list_resp,
                                     rsi_ble_on_profile_resp_t ble_on_profile_resp,
                                     rsi_ble_on_char_services_resp_t ble_on_char_services_resp,
                                     rsi_ble_on_inc_services_resp_t ble_on_inc_services_resp,
                                     rsi_ble_on_att_desc_resp_t ble_on_att_desc_resp,
                                     rsi_ble_on_read_resp_t ble_on_read_resp,
                                     rsi_ble_on_write_resp_t ble_on_write_resp,
                                     rsi_ble_on_gatt_write_event_t ble_on_gatt_event,
                                     rsi_ble_on_gatt_prepare_write_event_t ble_on_gatt_prepare_write_event,
                                     rsi_ble_on_execute_write_event_t ble_on_execute_write_event,
                                     rsi_ble_on_read_req_event_t ble_on_read_req_event,
                                     rsi_ble_on_mtu_event_t ble_on_mtu_event,
                                     rsi_ble_on_gatt_error_resp_t ble_on_gatt_error_resp_event,
                                     rsi_ble_on_gatt_desc_val_event_t ble_on_gatt_desc_val_resp_event,
                                     rsi_ble_on_event_profiles_list_t ble_on_profiles_list_event,
                                     rsi_ble_on_event_profile_by_uuid_t ble_on_profile_by_uuid_event,
                                     rsi_ble_on_event_read_by_char_services_t ble_on_read_by_char_services_event,
                                     rsi_ble_on_event_read_by_inc_services_t ble_on_read_by_inc_services_event,
                                     rsi_ble_on_event_read_att_value_t ble_on_read_att_value_event,
                                     rsi_ble_on_event_read_resp_t ble_on_read_resp_event,
                                     rsi_ble_on_event_write_resp_t ble_on_write_resp_event,
                                     rsi_ble_on_event_indicate_confirmation_t ble_on_indicate_confirmation_event,
                                     rsi_ble_on_event_prepare_write_resp_t ble_on_prepare_write_resp_event);

/*==============================================*/
/**
 * @fn         rsi_ble_gatt_extended_register_callbacks
 */
void rsi_ble_gatt_extended_register_callbacks(rsi_ble_on_mtu_exchange_info_t ble_on_mtu_exchange_info_event);

/*********************************************************************************
 * * BLE L2CAP Credit based flow control(CBFC) Callbacks register function Declarations
 * *******************************************************************************/

/**
 * @typedef void (*rsi_ble_on_cbfc_conn_req_event_t)(rsi_ble_event_cbfc_conn_req_t *rsi_ble_cbfc_conn_req);
 * @brief      Callback function to indicate L2CAP CBFC connection request
 * @param[in]  
 * @param[in]  rsi_ble_cbfc_conn_req,  contains the connection request information
 * @return      void
 * @section description
 * This callback function will be called when connected to indicate connection request
 * This callback has to be registered using rsi_ble_l2cap_cbfc_callbacks API
 */
typedef void (*rsi_ble_on_cbfc_conn_req_event_t)(rsi_ble_event_cbfc_conn_req_t *rsi_ble_cbfc_conn_req);

/**
 * @typedef void (*rsi_ble_on_cbfc_conn_complete_event_t)(rsi_ble_event_cbfc_conn_complete_t *rsi_ble_cbfc_conn_complete,
 *                                                     uint16_t status);
 * @brief      Callback function to indicate L2CAP CBFC connection complete status
 * @param[in]  
 * @param[in]  rsi_ble_cbfc_conn_complete,  contains the connection completed information
 * @return      void
 * @section description
 * This callback function will be called when connected to indicate connection complete status
 * This callback has to be registered using rsi_ble_l2cap_cbfc_callbacks API
 */
typedef void (*rsi_ble_on_cbfc_conn_complete_event_t)(rsi_ble_event_cbfc_conn_complete_t *rsi_ble_cbfc_conn_complete,
                                                      uint16_t status);

/**
 * @typedef void (*rsi_ble_on_cbfc_rx_data_event_t)(rsi_ble_event_cbfc_rx_data_t *rsi_ble_cbfc_rx_data);
 * @brief      Callback function to indicate L2CAP CBFC rx data event 
 * @param[in]  
 * @param[in]  rsi_ble_cbfc_rx_data,  contains the received data information
 * @return      void
 * @section description
 * This callback function will be called when connected to indicate received data
 * This callback has to be registered using rsi_ble_l2cap_cbfc_callbacks API
 */
typedef void (*rsi_ble_on_cbfc_rx_data_event_t)(rsi_ble_event_cbfc_rx_data_t *rsi_ble_cbfc_rx_data);

/**
 * @typedef void (*rsi_ble_on_cbfc_disconn_event_t)(rsi_ble_event_cbfc_disconn_t *rsi_ble_cbfc_disconn);
 * @brief      Callback function to indicate L2CAP CBFC disconnection evnet 
 * @param[in]  
 * @param[in]  rsi_ble_cbfc_disconn,  contains the disconnect device information
 * @return      void
 * @section description
 * This callback function will be called when connected to indicate disconnect l2cap connection
 * This callback has to be registered using rsi_ble_l2cap_cbfc_callbacks API
 */
typedef void (*rsi_ble_on_cbfc_disconn_event_t)(rsi_ble_event_cbfc_disconn_t *rsi_ble_cbfc_disconn);

/*==============================================*/
/**
 * @fn         rsi_ble_l2cap_cbsc_register_callbacks
 */
void rsi_ble_l2cap_cbsc_register_callbacks(rsi_ble_on_cbfc_conn_req_event_t ble_on_cbsc_conn_req,
                                           rsi_ble_on_cbfc_conn_complete_event_t ble_on_cbsc_conn_complete,
                                           rsi_ble_on_cbfc_rx_data_event_t ble_on_cbsc_rx_data,
                                           rsi_ble_on_cbfc_disconn_event_t ble_on_cbsc_disconn);

typedef void (*chip_ble_buffers_stats_handler_t)(chip_ble_buffers_stats_t *chip_ble_buffers_stats);
/**
 * @fn         rsi_ble_vendor_rf_type
 * @brief      This api gives vendor specific command to the controller to set rf type.
 *             RF mode will be decide based on the TXPOWER index. 
 * @return     int32_t
 *             0  =  success
 *             !0 = failure
 * @section description
 * This function is used to issue vendor specific command for setting rf type in controller on given inputs.
 * 
 */
int32_t rsi_ble_vendor_rf_type(uint8_t ble_power_index);

/*==============================================*/
/**
 * @fn         BT_LE_ADPacketExtract
 */
void BT_LE_ADPacketExtract(uint8_t *remote_name, uint8_t *pbuf, uint8_t buf_len);

int32_t rsi_ble_set_random_address_with_value(uint8_t *random_addr);
int32_t rsi_ble_set_random_address(void);
int32_t rsi_ble_set_smp_pairing_cap_data(rsi_ble_set_smp_pairing_capabilty_data_t *smp_pair_cap_data);
int32_t rsi_ble_conn_param_resp(uint8_t *remote_dev_address, uint8_t status);
int32_t rsi_ble_get_le_ping_timeout(uint8_t *remote_dev_address, uint16_t *time_out);
int32_t rsi_ble_set_le_ping_timeout(uint8_t *remote_dev_address, uint16_t time_out);
int32_t rsi_ble_clear_whitelist(void);
int32_t rsi_ble_addto_whitelist(int8_t *dev_address, uint8_t dev_addr_type);
int32_t rsi_ble_deletefrom_whitelist(int8_t *dev_address, uint8_t dev_addr_type);
int32_t rsi_ble_readphy(int8_t *remote_dev_address, rsi_ble_resp_read_phy_t *resp);
int32_t rsi_ble_setphy(int8_t *remote_dev_address, uint8_t tx_phy, uint8_t rx_phy, uint16_t coded_phy);
int32_t rsi_ble_conn_params_update(uint8_t *remote_dev_address,
                                   uint16_t min_int,
                                   uint16_t max_int,
                                   uint16_t latency,
                                   uint16_t timeout);
int32_t rsi_ble_set_data_len(uint8_t *remote_dev_address, uint16_t tx_octets, uint16_t tx_time);
int32_t rsi_ble_read_max_data_len(rsi_ble_read_max_data_length_t *blereaddatalen);
int32_t rsi_ble_rx_test_mode(uint8_t rx_channel, uint8_t phy, uint8_t modulation);
int32_t rsi_ble_tx_test_mode(uint8_t tx_channel, uint8_t phy, uint8_t tx_len, uint8_t mode);
int32_t rsi_ble_end_test_mode(uint16_t *num_of_pkts);
int32_t rsi_ble_per_transmit(struct rsi_ble_per_transmit_s *rsi_ble_per_tx);
int32_t rsi_ble_per_receive(struct rsi_ble_per_receive_s *rsi_ble_per_rx);
int32_t rsi_ble_start_encryption(uint8_t *remote_dev_address, uint16_t ediv, uint8_t *rand, uint8_t *ltk);

int32_t rsi_ble_notify_value(uint8_t *dev_addr, uint16_t handle, uint16_t data_len, uint8_t *p_data);

int32_t rsi_ble_indicate_value(uint8_t *dev_addr, uint16_t handle, uint16_t data_len, uint8_t *p_data);
int32_t rsi_ble_indicate_value_sync(uint8_t *dev_addr, uint16_t handle, uint16_t data_len, uint8_t *p_data);
int32_t rsi_ble_indicate_confirm(uint8_t *dev_addr);
int32_t rsi_ble_remove_gatt_service(uint32_t service_handler);
int32_t rsi_ble_remove_gatt_attibute(uint32_t service_handler, uint16_t att_hndl);
int32_t rsi_ble_att_error_response(uint8_t *dev_addr, uint16_t handle, uint8_t opcode, uint8_t err);

int32_t rsi_ble_gatt_write_response(uint8_t *dev_addr, uint8_t type);

int32_t rsi_ble_gatt_prepare_write_response(uint8_t *dev_addr,
                                            uint16_t handle,
                                            uint16_t offset,
                                            uint16_t length,
                                            uint8_t *data);

int32_t rsi_ble_mtu_exchange_event(uint8_t *dev_addr, uint8_t mtu_size);
int32_t rsi_ble_mtu_exchange_resp(uint8_t *dev_addr, uint8_t mtu_size);
void rsi_ble_gap_extended_register_callbacks(rsi_ble_on_remote_features_t ble_on_remote_features_event,
                                             rsi_ble_on_le_more_data_req_t ble_on_le_more_data_req_event);
int32_t rsi_ble_set_wo_resp_notify_buf_info(uint8_t *dev_addr, uint8_t buf_mode, uint8_t buf_cnt);
#endif
