/*******************************************************************************
* @file  rsi_bt_common.h
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

#if (defined(RSI_BT_ENABLE) || defined(RSI_BLE_ENABLE))

#ifndef RSI_BT_COMMON_H
#define RSI_BT_COMMON_H

#include <rsi_data_types.h>
#include <rsi_error.h>
#include <rsi_os.h>
#include <rsi_pkt_mgmt.h>
//#include<rsi_driver.h>

//#define  RSI_DEBUG_PRINTS
/******************************************************
 * *                      Macros
 * ******************************************************/
#define RSI_BT_HCI_CMD_PKT     0x1
#define RSI_BT_HCI_ACL_PKT     0x2
#define RSI_BT_HCI_PER_CMD_PKT 0x3

#define RSI_BT_DUAL_MODE     0x8
#define RSI_BT_BLE_MODE_BITS (BIT(2) | BIT(3))

#define RSI_BT_STATUS_OFFSET   0x0C
#define RSI_BT_RSP_TYPE_OFFSET 0x02
#define RSI_BT_RSP_LEN_OFFSET  0x00
#define RSI_BT_RSP_LEN_MASK    0x0FFF

#define RSI_PROTO_BT_COMMON  0x01
#define RSI_PROTO_BT_CLASSIC 0x02
#define RSI_PROTO_BLE        0x03
#ifdef RSI_PROP_PROTOCOL_ENABLE
#define RSI_PROTO_PROP_PROTOCOL 0x04
#endif
#define RSI_PROTO_BT_BLE_STACK 0x05

#define RSI_DEV_NAME_LEN 50
#define RSI_DEV_ADDR_LEN 6
#define RSI_DEV_ATT_LEN  240

#define RSI_BT_CLASSIC_DEVICE 0x00
#define RSI_BT_LE_DEVICE      0x01
#define RSI_BT_UNKWON_DEVICE  0xFF

#define STACK_BT_MODE   0x01
#define STACK_BTLE_MODE 0x02

//RF types
#define RSI_EXTERNAL_RF 0x00
#define RSI_INTERNAL_RF 0x01
#define RSI_RF_TYPE     RSI_INTERNAL_RF

#define RSI_MAX_ATT 5

#define RSI_OPERMODE_WLAN_BT_DUAL_MODE 9

#define RSI_BT_STATE_NONE          0
#define RSI_BT_STATE_OPERMODE_DONE 1
/******************************************************
 * *                    Constants
 * ******************************************************/

/******************************************************
 * *                 Type Definitions
 * ******************************************************/

typedef struct rsi_ble_cb_s rsi_ble_cb_t;
typedef struct rsi_bt_classic_cb_s rsi_bt_classic_cb_t;
typedef struct rsi_bt_common_specific_cb_s rsi_bt_common_specific_cb_t;
typedef int32_t (*rsi_bt_get_ber_pkt_t)(uint8_t *pkt, uint16_t pkt_len);
#ifdef RSI_PROP_PROTOCOL_ENABLE
typedef struct rsi_prop_protocol_cb_s rsi_prop_protocol_cb_t;
#endif

/******************************************************
 * *                   Enumerations
 * ******************************************************/

typedef enum rsi_bt_common_cmd_request_e {
  RSI_BT_SET_LOCAL_NAME                            = 0x0001,
  RSI_BT_GET_LOCAL_NAME                            = 0x0002,
  RSI_BT_GET_RSSI                                  = 0x0005,
  RSI_BT_GET_LOCAL_DEV_ADDR                        = 0x0007,
  RSI_BT_REQ_INIT                                  = 0x008D,
  RSI_BT_REQ_DEINIT                                = 0x008E,
  RSI_BT_SET_ANTENNA_SELECT                        = 0x008F,
  RSI_BT_REQ_PER_CMD                               = 0x009A,
  RSI_BT_SET_FEATURES_BITMAP                       = 0x00A6,
  RSI_BT_VENDOR_SPECIFIC                           = 0x00BE,
  RSI_BT_SET_ANTENNA_TX_POWER_LEVEL                = 0x00A7,
  RSI_BT_SET_GAIN_TABLE_OFFSET_OR_MAX_POWER_UPDATE = 0x012C,
  RSI_BT_SET_BD_ADDR_REQ                           = 0x012E,
  RSI_BT_GET_BT_STACK_VERSION                      = 0x012F,

  RSI_BLE_ONLY_OPER_MODE = 0x8010,
  RSI_BLE_REQ_PWRMODE    = 0x8015,
  RSI_BLE_REQ_SOFTRESET  = 0x801C
} rsi_bt_common_cmd_request_t;

typedef enum rsi_bt_common_event_e {
  RSI_BT_EVENT_CARD_READY = 0x0505,
} rsi_bt_common_event_t;

typedef enum rsi_bt_common_cmd_resp_e {
  RSI_BT_RSP_SET_LOCAL_NAME                            = 0x0001,
  RSI_BT_RSP_QUERY_LOCAL_NAME                          = 0x0002,
  RSI_BT_RSP_QUERY_RSSI                                = 0x0005,
  RSI_BT_RSP_QUERY_LOCAL_BD_ADDRESS                    = 0x0007,
  RSI_BT_RSP_INIT                                      = 0x008D,
  RSI_BT_RSP_DEINIT                                    = 0x008E,
  RSI_BT_RSP_ANTENNA_SELECT                            = 0x008F,
  RSI_BT_RSP_SET_FEATURES_BITMAP                       = 0x00A6,
  RSI_BT_RSP_ANTENNA_TX_POWER_LEVEL                    = 0x00A7,
  RSI_BT_RSP_SET_GAIN_TABLE_OFFSET_OR_MAX_POWER_UPDATE = 0x012C,
  RSI_BT_RSP_SET_BD_ADDR                               = 0x012E,

  RSI_BLE_RSP_ONLY_OPER_MODE = 0x8010,
  RSI_BLE_RSP_PWRMODE        = 0x8015,
  RSI_BLE_RSP_SOFTRESET      = 0x801C
} rsi_bt_common_cmd_resp_t;

/******************************************************
 * *                    Structures
 * ******************************************************/
// Driver control block
struct rsi_driver_cb_s;

// Driver BT Common control block
struct rsi_bt_common_specific_cb_s {
  // PER BER Call Backs
  rsi_bt_get_ber_pkt_t rsi_bt_get_ber_pkt;
};

// Specific BT, BLE blocks
typedef struct rsi_bt_global_cb_s {
  // BT Common specific cb
  rsi_bt_common_specific_cb_t *bt_common_specific_cb;
  // BLE specific cb
  rsi_ble_cb_t *ble_specific_cb;
  // BT Classic specific cb
  rsi_bt_classic_cb_t *bt_specific_cb;
#ifdef RSI_PROP_PROTOCOL_ENABLE
  // PROP_PROTOCOL specific cb
  rsi_prop_protocol_cb_t *prop_protocol_specific_cb;
#endif
} rsi_bt_global_cb_t;

// Remote LE Device info structure
typedef struct rsi_remote_ble_info_s {
  // BD Address of the remote LE device
  uint8_t remote_dev_bd_addr[RSI_DEV_ADDR_LEN];

  // Address type of the remote LE device
  uint8_t remote_dev_addr_type;

  // Available Buffer Count
  uint8_t avail_buf_cnt;

  // Max Buffer Count
  uint8_t max_buf_cnt;

  // Max Buffer Length
  uint16_t max_buf_len;

  // Flag for dev info used or not
  uint8_t used;

  // Flag for checking command in use
  uint8_t cmd_in_use;

  // Flag for checking expected remote response for each procedure
  uint16_t expected_resp;

  // Buffer config mode
  uint8_t mode;

  // mutex handle for avail_buf_info update
  rsi_mutex_handle_t ble_buff_mutex;
} rsi_remote_ble_info_t;

// Driver BT/BLE/PROP_PROTOCOL control block
typedef struct rsi_bt_cb_s {
  // driver BT control block status
  volatile int32_t status;

  // driver BT control block state
  uint16_t state;

  // driver BT control block mutex
  rsi_mutex_handle_t bt_mutex;

  // driver BT control block expected command response
  void *expected_response_buffer;

  // expected command response type
  uint16_t expected_response_type;

  // sync command flag to identify that the command is blocking / sync type
  uint8_t sync_rsp;

  // BT device type at disconnect.
  uint8_t dev_type;

  // driver BT control block semaphore
  rsi_semaphore_handle_t bt_cmd_sem;

  // driver BT control block semaphore
  rsi_semaphore_handle_t bt_sem;

  // driver BT control block tx packet pool
  rsi_pkt_pool_t bt_tx_pool;

  // buffer pointer given by application to driver
  uint8_t *app_buffer;

  // buffer length given by application to driver
  uint32_t app_buffer_length;

  rsi_bt_global_cb_t *bt_global_cb;

  //uint8, address of the device to which directed advertising has to be done in ll privacy mode
  uint8_t directed_addr[RSI_DEV_ADDR_LEN];

// Structure Holding Remote LE Dev info (BD address & Controller Buffer availability)
#define MAX_REMOTE_BLE_DEVICES 8
  rsi_remote_ble_info_t remote_ble_info[MAX_REMOTE_BLE_DEVICES];

  // Variable indicating buffer full/empty status --> 0 -> Empty, 1 -> Full
  uint8_t buf_status;

  // Variable indicating command in use status --> 0 -> Not In Use, 1 -> In Use
  uint8_t cmd_status;

  // Variable to save Remote info index
  uint8_t remote_ble_index;
} rsi_bt_cb_t;

// Set local name command structure
typedef struct rsi_bt_req_set_local_name_s {
  // uint8, length of the required name to be set
  uint8_t name_len;
  // int8[50], required name
  int8_t name[RSI_DEV_NAME_LEN];
} rsi_bt_req_set_local_name_t;

// Get RSSI command structure
typedef struct rsi_bt_get_rssi_s {
  // uint8, device address of the device whose RSSI has to be known
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
} rsi_bt_get_rssi_t;

// Response structures

// Get local name response structures
typedef struct rsi_bt_resp_get_local_name_s {
  /** Name length */
  uint8_t name_len;
  /** Array which consists name of the local device. The maximum size of this array is 50 */
  int8_t name[RSI_DEV_NAME_LEN];
} rsi_bt_resp_get_local_name_t;

// Get Stack Version
typedef struct rsi_bt_resp_get_bt_stack_version_s {
  /** stack version variable */
  int8_t stack_version[10];
} rsi_bt_resp_get_bt_stack_version_t;

// Set antenna structure
typedef struct rsi_ble_set_antenna_s {
  //uint8, antenna value - internal/external
  uint8_t value;
} rsi_ble_set_antenna_t;

// Set feature bitmap structure
typedef struct rsi_bt_set_feature_bitmap_s {
  //uint8[4], features bits (bit map)
  uint32_t bit_map;
} rsi_bt_set_feature_bitmap_t;

typedef struct rsi_ble_oper_mode_s {
  //uint8, antenna value - internal/external
  uint32_t bt_features;
  uint8_t module_type;
} rsi_ble_oper_mode_t;

typedef struct rsi_ble_power_mode_s {
  // power mode to set
  uint8_t power_mode;

  // set LP/ULP/ULP-without RAM retention
  uint8_t ulp_mode_enable;

} rsi_ble_power_mode_t;

// Set antenna tx power level structure
typedef struct rsi_bt_set_antenna_tx_power_level_s {
  //uint8, protocol_mode : 1-BT, 2-LE
  uint8_t protocol_mode;
  //int8_t, transmit power
  int8_t tx_power;
} rsi_bt_set_antenna_tx_power_level_t;

// BT PER Stats
typedef struct rsi_bt_per_stats_s {
  /** Packet count of CRC fails (Cyclic Redundancy Check (CRC)) */
  uint16_t crc_fail_cnt;
  /** Packet count of CRC fails (Cyclic Redundancy Check (CRC)) */
  uint16_t crc_pass_cnt;
  /** Packet count of aborted Tx */
  uint16_t tx_abort_cnt;
  /** Packet count of dropped Rx */
  uint16_t rx_drop_cnt;
  /** Packet count of CCA Idle (Clear Channel Assessment (CCA)) */
  uint16_t rx_cca_idle_cnt;
  /** Packet count of Rx start */
  uint16_t rx_start_idle_cnt;
  /** Packet count of aborted Rx */
  uint16_t rx_abrt_cnt;
  /** Packet count of successful transmissions */
  uint16_t tx_dones;
  /** Received Signal Strength Indicator of the packet */
  int8_t rssi;
  /** Packet count of ID packets received */
  uint16_t id_pkts_rcvd;
  /** Dummy array of length 5 */
  uint16_t dummy[5];
} rsi_bt_per_stats_t;

typedef struct rsi_bt_per_cw_mode_s {
  /** Command ID. Should be set to #define HCI_BT_CW_MODE_CMD_ID   0x0D*/
  uint8_t cmd_id;
  /** 1-Enable, 0-Disable */
  uint8_t cw_mode_enable;
} rsi_bt_per_cw_mode_t;

typedef struct rsi_bt_set_local_bd_addr_s {
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
} rsi_bt_set_local_bd_addr_t;

// BT BER CMD
typedef struct rsi_bt_ber_cmd_s {
  /** BER command ID */
  uint8_t cmd_id;
  /** 1-Enable, 0-Disable */
  uint8_t enable;
} rsi_bt_ber_cmd_t;

typedef struct rsi_bt_cmd_update_gain_table_offset_or_maxpower_s {
  /** node id (0 - BLE, 1 - BT) */
  uint8_t node_id;
  /** gain table request type (0 - max power update, 1 - offset update) */
  uint8_t update_gain_table_type;
  /** gain table payload length */
  uint8_t payload_len;
  /** gain table payload data */
  uint8_t payload[128];
} rsi_bt_cmd_update_gain_table_offset_or_maxpower_t;

/******************************************************
 * * BT/BLE common function declarations
 * ******************************************************/
void rsi_bt_set_status(rsi_bt_cb_t *bt_cb, int32_t status);
void rsi_bt_common_init(void);
void rsi_bt_common_tx_done(rsi_pkt_t *pkt);
int8_t rsi_bt_cb_init(rsi_bt_cb_t *bt_cb, uint16_t protocol_type);
int32_t rsi_bt_driver_send_cmd(uint16_t cmd, void *cmd_struct, void *resp);
uint16_t rsi_bt_global_cb_init(struct rsi_driver_cb_s *driver_cb, uint8_t *buffer);
uint16_t rsi_driver_process_bt_resp_handler(rsi_pkt_t *pkt);
uint16_t rsi_bt_get_proto_type(uint16_t rsp_type, rsi_bt_cb_t **bt_cb);
uint8_t rsi_bt_get_ACL_type(uint16_t rsp_type);

int32_t rsi_bt_get_local_name(rsi_bt_resp_get_local_name_t *bt_resp_get_local_name);
int32_t rsi_bt_per_cw_mode(struct rsi_bt_per_cw_mode_s *bt_cw_mode);
#ifndef SAPIS_BT_STACK_ON_HOST
int32_t intialize_bt_stack(uint8_t mode);
#endif
#endif

#define HCI_BT_PER_STATS_CMD_ID 0x08
#define HCI_BT_CW_MODE_CMD_ID   0x0D
#define HCI_BLE_TRANSMIT_CMD_ID 0x13
#define HCI_BLE_RECEIVE_CMD_ID  0x14
#define HCI_BT_TRANSMIT_CMD_ID  0x15
#define HCI_BT_RECEIVE_CMD_ID   0x16

#endif
