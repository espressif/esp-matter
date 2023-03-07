/*******************************************************************************
* @file  rsi_bt_apis.h
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

/*************************************************************************
 *
 */

#ifndef RSI_BT_APIS_H
#define RSI_BT_APIS_H

#include <rsi_data_types.h>
#include <rsi_utils.h>
#include <rsi_bt_common.h>
#ifdef RSI_BT_ENABLE
#include <rsi_bt_config.h>
#else
#include <rsi_bt_common_config.h>
#endif
#include <rsi_bt_sdp.h>

/******************************************************
 * *                      Macros
 * ******************************************************/
#define RSI_LINK_KEY_LEN 16

#define RSI_BUTTON_PUSHED   0x0
#define RSI_BUTTON_RELEASED 0x1
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

//BT Classic GAP Command structures

typedef struct rsi_bt_req_query_services_s {
  /** remote device address */
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
} rsi_bt_req_query_services_t;

//BT Classic GAP Response structures

typedef struct rsi_bt_resp_query_services_s {
  /** Number of Services Supported by Remote Device*/
  uint8_t num_of_services;

  /** Reserved For Future Use*/
  uint8_t reserved[3];

  /** List Of Services with 32 bit UUID*/
  uint32_t uuid[32];
} rsi_bt_resp_query_services_t;

typedef struct rsi_bt_resp_search_service_s {
  uint8_t service_status;
} rsi_bt_resp_search_service_t;

//BT Classic GAP Event structures

typedef struct rsi_bt_event_role_change_s {

  /** remote device address */
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];

  /** Currenty role of the local device */
  uint8_t role;
} rsi_bt_event_role_change_t;

typedef struct rsi_bt_event_unbond_s {

  /** rsi_bt_event_unbond_s */
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
} rsi_bt_event_unbond_t;

typedef struct rsi_bt_event_bond_response_s {

  /** remote device address*/
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
} rsi_bt_event_bond_t;

typedef struct rsi_bt_event_inquiry_response_s {

  /**	Inquiry scan type */
  uint8_t inquiry_type;

  /** remote device address */
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];

  /** 	Name length of the remote device */
  uint8_t name_length;

  /** Name of remote device with max length of 50 bytes */
  uint8_t remote_device_name[RSI_DEV_NAME_LEN];

  /** 	Class of Device */
  uint8_t cod[3];

  /** RSSI of the remote device */
  int8_t rssi;
} rsi_bt_event_inquiry_response_t;

typedef struct rsi_bt_event_remote_device_name_s {
  /** Remote Device BD Address*/
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];

  /** Length Of the Remote Device Name*/
  uint8_t name_length;

  /** Name of the Remote Device*/
  uint8_t remote_device_name[RSI_DEV_NAME_LEN];
} rsi_bt_event_remote_device_name_t;

typedef struct rsi_bt_event_remote_name_request_cancel_s {

  /** remote device address */
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
} rsi_bt_event_remote_name_request_cancel_t;

typedef struct rsi_bt_event_connection_initiated_s {

  /** if the value is 1 implies connection initiated by remote device */
  uint8_t conn_initiated;
} rsi_bt_event_connection_initiated_t;

typedef struct rsi_bt_event_disconnect_s {

  /** remote device address */
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
} rsi_bt_event_disconnect_t;

typedef struct rsi_bt_event_user_confirmation_request_s {

  /** remote device address */
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];

  /** 	Reserved field for future use */
  uint8_t reserved[2];

  /** This is the passkey to be confirmed */
  uint8_t confirmation_value[4];
} rsi_bt_event_user_confirmation_request_t;

typedef struct rsi_bt_event_user_passkey_display_s {

  /** remote device address*/
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];

  /** Reserved field for future use */
  uint8_t reserved[2];

  /** Passkey value */
  uint8_t passkey[4];
} rsi_bt_event_user_passkey_display_t;

typedef struct rsi_bt_event_user_passkey_notify_s {
  uint8_t dev_addr[6];
  uint8_t reserved[2];
  uint8_t passkey[4];
} rsi_bt_event_user_passkey_notify_t;

typedef struct rsi_bt_event_user_pincode_request_s {

  /** remote device address */
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
} rsi_bt_event_user_pincode_request_t;

typedef struct rsi_bt_event_user_passkey_request_s {

  /** remote device address*/
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
} rsi_bt_event_user_passkey_request_t;

typedef struct rsi_bt_event_auth_complete_s {

  /** remote device address */
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
} rsi_bt_event_auth_complete_t;

typedef struct rsi_bt_event_user_linkkey_request_s {

  /** remote device address */
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
} rsi_bt_event_user_linkkey_request_t;

typedef struct rsi_bt_event_user_linkkey_save_s {

  /** remote device address */
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];

  /** Link Key to be saved */
  uint8_t linkKey[RSI_LINK_KEY_LEN];

  /** local device address */
  uint8_t local_dev_addr[RSI_DEV_ADDR_LEN];
} rsi_bt_event_user_linkkey_save_t;

typedef struct rsi_bt_event_spp_receive_s {

  /** Length of the data received */
  uint16_t data_len;

  /** Buffer holding the received SPP data. It holds the max payload length of 1k bytes */
  uint8_t data[RSI_BT_MAX_PAYLOAD_SIZE];
} rsi_bt_event_spp_receive_t;

typedef struct rsi_bt_event_spp_connect_s {
  /** remote device address */
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];

  /** MTU size supported by the remote device */
  uint16_t tx_mtu_size;
  uint16_t rx_mtu_size;

} rsi_bt_event_spp_connect_t;

typedef struct rsi_bt_event_spp_disconnect_s {

  /** remote device address*/
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
} rsi_bt_event_spp_disconnect_t;

typedef struct rsi_bt_event_l2cap_data_s {
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
  uint8_t data_len[2];
  uint8_t data[RSI_BT_MAX_PAYLOAD_SIZE];
} rsi_bt_event_l2cap_data_t;
typedef struct rsi_bt_event_l2cap_connect_s {
#define BT_L2CAP_CONNECTED    1
#define BT_L2CAP_DISCONNECTED 0
  uint8_t conn_status;
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
  uint8_t reason_code[2];
  uint8_t rem_mtu_size[2];
} rsi_bt_event_l2cap_connect_t;

typedef struct rsi_bt_event_l2cap_disconnect_s {
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
} rsi_bt_event_l2cap_disconnect_t;

typedef struct rsi_bt_event_hid_receive_s {
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
  uint8_t hid_channel;
  uint8_t data_len[2];
  uint8_t data[RSI_BT_MAX_PAYLOAD_SIZE];
} rsi_bt_event_hid_receive_t;

typedef struct rsi_bt_event_hid_connect_s {
#define BT_HID_CONNECTED    1
#define BT_HID_DISCONNECTED 0
  uint8_t conn_status;
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
  uint8_t reason_code[2];
  uint8_t rem_mtu_size[2];
} rsi_bt_event_hid_connect_t;

typedef struct rsi_bt_event_hid_disconnect_s {
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
} rsi_bt_event_hid_disconnect_t;

typedef struct rsi_bt_event_mode_change_s {

  /** remote device address */
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];

  /** Indicates the current state of connection between the local device and the remote device i.e., Active mode / Hold mode/ Sniff mode \n 
      0 ? Active Mode \n 
      1 ? Hold Mode  \n 
      2 ? Sniff Mode */
  uint8_t current_mode;

  /** Reserved field for future use */
  uint8_t reserved;

  /** This specifies the time interval to each mode */
  uint16_t mode_interval;
} rsi_bt_event_mode_change_t;

typedef struct rsi_bt_event_sniff_subrating_s {

  /** remote device address */
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];

  /** Maximum latency for data being transmitted from the local device to the remote device */
  uint16_t max_tx_latency;

  /** Base sniff subrate timeout in baseband slots that the remote device use */
  uint16_t min_remote_timeout;

  /** Base sniff subrate timeout in baseband slots that the local device use */
  uint16_t min_local_timeout;
} rsi_bt_event_sniff_subrating_t;

typedef struct rsi_bt_event_gatt_connect_s {
  /** remote device address */
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
} rsi_bt_event_gatt_connect_t;

typedef struct rsi_bt_event_gatt_disconnect_s {
  /** remote device address */
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
} rsi_bt_event_gatt_disconnect_t;

typedef struct rsi_bt_event_ssp_complete_s {

  /** remote device address */
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];

  /** This is the SSP mode connection status with remote device */
  uint8_t status;
} rsi_bt_event_ssp_complete_t;

//PER structure paramters
typedef struct rsi_bt_tx_per_params_s {
  /** Command ID parameter is set to BT_TRANSMIT_CMD_ID of value 0x15 */
  uint8_t cmd_id;
  /** enables/disables the bt per transmit mode \n
  1 ? PER transmit Enable \n
  0 ? PER transmit Disable */
  uint8_t transmit_enable;
  /** Device BD address from which packets to be transmitted */
  uint8_t device_Addr[6];
  /** Length of the packet to be transmitted */
  uint8_t pkt_len[2];
  /** Packet types proposed by BT_SIG. Value takes from 0 to 15 */
  uint8_t pkt_type;
  /** Field corresponds to BR/EDR Mode \n
  1 ? BR_MODE \n
  2 ? EDR_MODE */
  uint8_t br_edr_mode;
  /** Field corresponds to rx channel number to be used   (0 to 78) */
  uint8_t rx_chnl_in;
  /** Field corresponds to tx channel number to be used (0 to 78) */
  uint8_t tx_chnl_in;
  /** field corresponds to link type to be setup for transmitting per packets \n
  0 ? SCO_LINK \n
  1 ? ACL_LINK \n
  2 ? ESCO_LINK */
  uint8_t link_type;
  /** Initial seed to be used for whitening. It should be set to �0� in order to disable whitening. \n
  In order to enable, one should give the scrambler seed value which is used on the receive side */
  uint8_t scrambler_seed;
  /** Defines the frequency hopping type to be used \n
  0 ? NO_HOPPING \n
  1 ? FIXED_HOPPING \n
  2 ? RANDOM_HOPPING (rx_chnl_in, tx_chnl_in parameters are unused in this mode) */
  uint8_t hopping_type;
  /** Field defines the antenna selection (onboard/external) to be used for transmission \n
  2 ? ONBOARD_ANT_SEL \n
  3 ? EXT_ANT_SEL */
  uint8_t ant_sel;
  /** Field defines the pll_mode type to be used \n
  0 ? PLL_MODE0 (to be used by Default) \n
  1 ? PLL_MODE1 */
  uint8_t pll_mode;
  /** Field defines the selection of RF type (internal/external) \n
  0 ? BT_EXTERNAL_RF \n
  1 ? BT_INTERNAL_RF (to be used by Default) */
  uint8_t rf_type;
  /** Field corresponds to the selection of RF chain (HP/LP) to be used \n
  2 ? BT_HP_CHAIN \n
  3 ? BT_LP_CHAIN (Only for BR_MODE) */
  uint8_t rf_chain;
  /**Field corresponds to the payload sequence of data to be transmitted. The default payload type is '4'. \n
  0 ? SEQUENCE_0 \n
  1 ? SEQUENCE_1 \n
  2 ? SEQUENCE_2 \n
  3 ? SEQUENCE_F0 \n
  4 ? SEQUENCE_PRBS */
  uint8_t payload_type;
  /** Field corresponds to the transmit power. Range is different based on rf_chain \n
  BT_LP_CHAIN -  1 - 31 power index - 0 dBm Mode \n
  33 - 63 power index - 8 dBm Mode \n
  Following are the equations to be used for deriving power output in dBm \n
  (0 - 31) o/p power equation is   -2 + 10log10(power_index/31) \n
  (32-63) o/p power equation is   -2 + 8+10log10((power_index -32)/31) \n \n
  BT_HP_CHAIN -  1 to 22 dBm (based on Country Regulations and Chip capability Max dBm varies. Configure 127 to select Max permitted value) */
  uint8_t tx_power;
  /** field corresponds to the transmit mode to be used either Burst/Continuous \n
  0 ? BURST_MODE  (BT stats are observed only in this mode) \n
  1 ? CONTINUOUS_MODE (no_of_packets variable is unused when this mode is selected) \n
  2 ? CONTINUOUS_WAVE_MODE (For CW_MODE) */
  uint8_t transmit_mode;
  /** This field takes the value of inter packet gap. \n
  Number of slots to be skipped between two packets. Each slot will be 625usec */
  uint8_t inter_pkt_gap;
  /** This field defines the number of packets to be transmitted, default to zero for continuous transmission */
  uint8_t no_of_packets[4];
} rsi_bt_tx_per_params_t;

//PER structure paramters
typedef struct rsi_bt_rx_per_params_s {
  /** Command ID parameter is set to BT_RECEIVE_CMD_ID of value 0x16 */
  uint8_t cmd_id;
  /** enables/disables the bt per receive mode \n
  1 ? PER Receive Enable \n
  0 ? PER Receive Disable */
  uint8_t receive_enable;
  /** Device BD address from which packets to be received */
  uint8_t device_Addr[6];
  /** Length of the packet to be received */
  uint8_t pkt_len[2];
  /** Packet types proposed by BT_SIG. Value takes from 0 to 15 */
  uint8_t pkt_type;
  /** Field corresponds to BR/EDR Mode \n
  1 ? BR_MODE \n
  2 ? EDR_MODE */
  uint8_t br_edr_mode;
  /** Field corresponds to rx channel number to be used for receive (0 to 78) */
  uint8_t rx_chnl_in;
  /** Field corresponds to tx channel number to be used (0 to 78) */
  uint8_t tx_chnl_in;
  /** field corresponds to link type to be setup for receiving per packets \n
  0 ? SCO_LINK \n
  1 ? ACL_LINK \n
  2 ? ESCO_LINK */
  uint8_t link_type;
  /** Initial seed to be used for whitening. It should be set to �0� in order to disable whitening. \n
  In order to enable, one should give the scrambler seed value which is used on the transmit side */
  uint8_t scrambler_seed;
  /** Defines the frequency hopping type to be used \n
  0 ? NO_HOPPING \n
  1 ? FIXED_HOPPING \n
  2 ? RANDOM_HOPPING (rx_chnl_in, tx_chnl_in parameters are unused in this mode) */
  uint8_t hopping_type;
  /** Field defines the antenna selection (onboard/external) to be used for reception \n
  2 ? ONBOARD_ANT_SEL \n
  3 ? EXT_ANT_SEL */
  uint8_t ant_sel;
  /** Field defines the pll_mode type to be used \n
  0 ? PLL_MODE0 (to be used by Default) \n
  1 ? PLL_MODE1 */
  uint8_t pll_mode;
  /** Field defines the selection of RF type (internal/external) \n
  0 ? BT_EXTERNAL_RF \n
  1 ? BT_INTERNAL_RF (to be used by Default) */
  uint8_t rf_type;
  /** Field corresponds to the selection of RF chain (HP/LP) to be used \n
  2 ? BT_HP_CHAIN \n
  3 ? BT_LP_CHAIN (Only for BR_MODE) */
  uint8_t rf_chain;
  /** Field defines the loopback to be enable or disable \n
  0 ? LOOP_BACK_MODE_DISABLE \n
  1 ? LOOP_BACK_MODE_ENABLE */
  uint8_t loop_back_mode;
} rsi_bt_rx_per_params_t;

typedef struct rsi_bt_event_avdtp_stats_s {

  /** LMAC timer BT Clock*/
  uint32_t clock;
  /* BDR stats */

  /** Num of TXed POLLS*/
  uint16_t tx_poll;

  /** Num of TXed NULLS*/
  uint16_t tx_null;

  /** Num of RXed POLLS*/
  uint16_t rx_poll;

  /** Num of RXed NULLS*/
  uint16_t rx_null;

  /** dm1_pkt[0] - Num of TXed dm1 Pkts \n
  dm1_pkt[1] - Num of RXed dm1 Pkts*/
  uint16_t dm1_pkt[2];

  /** dm3_pkt[0] - Num of TXed dm3 Pkts \n
  dm3_pkt[1] - Num of RXed dm3 Pkts*/
  uint16_t dm3_pkt[2];

  /** dm5_pkt[0] - Num of TXed dm5 Pkts \n
  dm5_pkt[1] - Num of RXed dm5 Pkts*/
  uint16_t dm5_pkt[2];

  /** dh1_pkt[0] - Num of TXed dh1 Pkts \n
  dh1_pkt[1] - Num of RXed dh1 Pkts*/
  uint16_t dh1_pkt[2];

  /** dh3_pkt[0] - Num of TXed dh3 Pkts \n
  dh3_pkt[1] - Num of RXed dh3 Pkts*/
  uint16_t dh3_pkt[2];

  /** dh5_pkt[0] - Num of TXed dh5 Pkts \n
  dh5_pkt[1] - Num of RXed dh5 Pkts*/
  uint16_t dh5_pkt[2];

  /* EDR stats */
  /** 2dh1_pkt[0] - Num of TXed 2dh1 Pkts \n
  2dh1_pkt[1] - Num of RXed 2dh1 Pkts*/
  uint16_t edr_2dh1_pkt[2];

  /** 2dh3_pkt[0] - Num of TXed 2dh3 Pkts \n
  2dh3_pkt[1] - Num of RXed 2dh3 Pkts*/
  uint16_t edr_2dh3_pkt[2];

  /** 2dh5_pkt[0] - Num of TXed 2dh5 Pkts \n
  2dh5_pkt[1] - Num of RXed 2dh5 Pkts*/
  uint16_t edr_2dh5_pkt[2];

  /** 3dh1_pkt[0] - Num of TXed 3dh1 Pkts \n
  3dh1_pkt[1] - Num of RXed 3dh1 Pkts*/
  uint16_t edr_3dh1_pkt[2];

  /** 3dh3_pkt[0] - Num of TXed 3dh3 Pkts \n
  3dh3_pkt[1] - Num of RXed 3dh3 Pkts*/
  uint16_t edr_3dh3_pkt[2];

  /** 3dh5_pkt[0] - Num of TXed 3dh5 Pkts \n
  3dh5_pkt[1] - Num of RXed 3dh5 Pkts*/
  uint16_t edr_3dh5_pkt[2];
  /* Packet stats overall */

  /** Num of CRC Failed Packets*/
  uint16_t no_of_crc_fail;

  /** Num of Retransmission Packets*/
  uint16_t no_of_retries;

  /** Num of Header Error Packets*/
  uint16_t no_of_hdr_err;

  /** Num of Duplicate Packets Received \n
  (If previous sequence number is received again)*/
  uint16_t no_of_seq_repeat;

  /** Num of ack packets received with delay of 10ms to 100ms and more that 100ms \n
  10 - ack_delay[10] ......... >100 - ack_delay[10]*/
  uint16_t ack_delay[11];

  /** Num of ACK packets Received*/
  uint16_t ack_received;

  /** Num of NAK packets Received*/
  uint16_t nak_received;

  /** Num of missed response packets in the RX Slot*/
  uint16_t no_of_rx_missed;

  /** Num of RXed error packets in the RX Slot*/
  uint16_t no_of_rx_error;

  /** Num of TX Attempts*/
  uint16_t tx_attempts;

  /** Number of Packets TXed on Air*/
  uint16_t tx_done;
} rsi_bt_event_avdtp_stats_t;

#define _3M_STATE_CONTINUOUS_FAIL_THRESHOLD_REACHED 1
#define _3M_STATE_PER_THRESHOLD_REACHED             2
#define _2M_STATE_PASS_THRESHOLD_REACHED            3
#define _2M_STATE_CONTINUOUS_PASS_THRESHOLD_REACHED 4
#define _BT_AR_STATS_PERIODIC                       5

typedef struct rsi_bt_event_ar_stats_s {

  /** 3m TX Fail Count*/
  uint32_t fail_count_3m;

  /** 2m TX Fail Count*/
  uint32_t fail_count_2m;

  /** 3m TX Pass Count*/
  uint32_t pass_count_3m;

  /** 2m TX Pass Count*/
  uint32_t pass_count_2m;

  /**#define _3M_STATE_CONTINUOUS_FAIL_THRESHOLD_REACHED 1 \n
     #define _3M_STATE_PER_THRESHOLD_REACHED             2 \n 
     #define _2M_STATE_PASS_THRESHOLD_REACHED            3 \n 
     #define _2M_STATE_CONTINUOUS_PASS_THRESHOLD_REACHED 4 \n 
     #define _BT_AR_STATS_PERIODIC                       5*/
  uint32_t reason_code;

  /** Current data rate state \n 
  #define STATE_3M_RATE  BIT(0) \n 
  #define STATE_2M_RATE  BIT(1)*/
  uint32_t current_state;
} rsi_bt_event_ar_stats_t;

typedef struct rsi_bt_event_pkt_change_s {

  /** remote device address*/
  uint8_t bd_addr[6];

  /** packet_type bitmap. \n 
  0x0001 Reserved for future use \n 
  0x0002 2-DH1 should not be used. \n 
  0x0004 3-DH1 should not be used \n 
  0x0008 DM1 may be used. \n 
  0x0010 DH1 may be used. \n 
  0x0020 Reserved for future use. \n 
  0x0040 Reserved for future use. \n 
  0x0080 Reserved for future use. \n 
  0x0100 2-DH3 should not be used. \n 
  0x0200 3-DH3 should not be used. \n 
  0x0400 DM3 may be used. \n 
  0x0800 DH3 may be used. \n
  0x1000 2-DH5 should not be used. \n 
  0x2000 3-DH5 should not be used. \n 
  0x4000 DM5 may be used. \n 
  0x8000 DH5 may be used.  */
  uint16_t pkt_type;

} rsi_bt_event_pkt_change_t;

typedef struct chip_bt_buffers_stats_s {
  uint8_t acl_buffer_utilization;
  uint8_t cp_buffer_utilization;
} chip_bt_buffers_stats_t;

typedef struct rsi_bt_event_iap_s {
  uint8_t status;
} rsi_bt_event_iap_t;

typedef struct rsi_bt_event_iap2_receive_s {
  uint16_t data_len;
  uint8_t data[1000];
} rsi_bt_event_iap2_receive_t;

/******************************************************
 * *       BT GAP Response Callbacks Declarations
 * ******************************************************/
/** @addtogroup BT-CLASSIC7
* @{
*/
/**
 * @typedef   void (*rsi_bt_on_role_change_t)(uint16_t resp_status, rsi_bt_event_role_change_t *role_change_status);
 * @brief       When role change status event is received from the module this callback will be called.
 * @param[out]  resp_status, contains the response status (Success or Error code) \n
 *               0              - SUCCESS \n
 *               Non-Zero Value - ErrorCodes
 * @note        Refer Bluetooth Generic Error Codes section  upto 0x4FF8 from \ref error-codes. 
 * @param[out]  role_change_status, contains the role_change_status information , please refer \ref rsi_bt_event_role_change_s structure for more information.
 * @return      void
 * @note        This callback has to be registered using rsi_bt_register_gap_callbacks API
 */
typedef void (*rsi_bt_on_role_change_t)(uint16_t resp_status, rsi_bt_event_role_change_t *role_change_status);

/**
 * @typedef   void (*rsi_bt_on_connect_t)(uint16_t resp_status, rsi_bt_event_bond_t *bond_response);
 * @brief       When  Connect event is raised from the module, this callback will be called. \n
 *              This event will be given by the module when BT PHY Level connection happens from either side.
 * @param[out]  resp_status, contains the response status (Success or Error code) \n
 *               0              - SUCCESS \n
 *               Non-Zero Value - ErrorCodes
 * @note        Refer Bluetooth Generic Error Codes section  upto 0x4FF8 from \ref error-codes 
 * @param[out]  bond_response, contains the connect/bond response information , please refer \ref rsi_bt_event_bond_response_s  structure for more information.
 * @return      void
 * @note        This callback has to be registered using rsi_bt_register_gap_callbacks API
 */
typedef void (*rsi_bt_on_connect_t)(uint16_t resp_status, rsi_bt_event_bond_t *bond_response);

/**
 * @typedef   void (*rsi_bt_on_unbond_t)(uint16_t resp_status, rsi_bt_event_unbond_t *unbond_status);
 * @brief       When unbond event is raised from the module this callback is called . \n 
                This event will be given by the module when either slave or master device issues unbond command to the other.
 * @param[out]  resp_status, contains the response status (Success or Error code) \n
 *               0              - SUCCESS \n
 *               Non-Zero Value - ErrorCodes
 * @note        Refer Bluetooth Generic Error Codes section  upto 0x4FF8 from \ref error-codes 
 * @param[out]  unbond_status, contains the disconnection information , please refer \ref rsi_bt_event_unbond_s structure for more information.
 * @return      void
 * @note        This callback has to be registered using rsi_bt_register_gap_callbacks API
 */
typedef void (*rsi_bt_on_unbond_t)(uint16_t resp_status, rsi_bt_event_unbond_t *unbond_status);

/**
 * @typedef   void (*rsi_bt_on_disconnect_t)(uint16_t resp_status, rsi_bt_event_disconnect_t *bt_disconnect);
 * @brief       When disconnection event is raised from the module this callback will be called . \n 
 *              This event will be given by the module when either slave or master device issues disconnect command to the other.
 * @param[out]  resp_status, contains the response status (Success or Error code) \n
 *               0              - SUCCESS \n
 *               Non-Zero Value - ErrorCodes
 * @note        Refer Bluetooth Generic Error Codes section  upto 0x4FF8 from \ref error-codes 
 * @param[out]  bt_disconnect, contains the disconnection information , please refer \ref rsi_bt_event_disconnect_s structure for more information.
 * @return      void
 * @note       This callback has to be registered using rsi_bt_register_gap_callbacks API
 */
typedef void (*rsi_bt_on_disconnect_t)(uint16_t resp_status, rsi_bt_event_disconnect_t *bt_disconnect);

/**
 * @typedef   void (*rsi_bt_on_scan_resp_t)(uint16_t resp_status, rsi_bt_event_inquiry_response_t *single_scan_resp);
 * @brief       When the single scan response is received from the module in response to inquiry command this callback is called .
 * @param[out]  resp_status, contains the response status (Success or Error code) \n
 *               0              - SUCCESS \n
 *               Non-Zero Value - ErrorCodes
 * @note        Refer Bluetooth Generic Error Codes section  upto 0x4FF8 from \ref error-codes 
 * @param[out]  rsi_ble_resp_profiles, contains the inquiry response information , please refer \ref rsi_bt_event_inquiry_response_s structure for more information.
 * @return      void
 * @note        This callback has to be registered using rsi_bt_register_gap_callbacks API
 */
typedef void (*rsi_bt_on_scan_resp_t)(uint16_t resp_status, rsi_bt_event_inquiry_response_t *single_scan_resp);

/**
 * @typedef    void (*rsi_bt_on_remote_name_resp_t)(uint16_t resp_status, rsi_bt_event_remote_device_name_t *name_resp);
 * @brief       When the remote name request command response is received from the module this callback is called.
 * @param[out]  resp_status, contains the response status (Success or Error code) \n 
 *               0              - SUCCESS \n
 *               Non-Zero Value - ErrorCodes
 * @note        Refer Bluetooth Generic Error Codes section  upto 0x4FF8 from \ref error-codes 
 * @param[out]  name_resp, contains the remote device name information, please refer \ref rsi_bt_event_remote_device_name_s structure for more information.
 * @return      void
 * @note        This callback has to be registered using rsi_bt_register_gap_callbacks API
 */
typedef void (*rsi_bt_on_remote_name_resp_t)(uint16_t resp_status, rsi_bt_event_remote_device_name_t *name_resp);

/**
 * @typedef   typedef void (*rsi_bt_on_passkey_display_t)(uint16_t resp_status,
                                            rsi_bt_event_user_passkey_display_t *bt_event_user_passkey_display);
 * @brief       When the passkey display request is received from the module this callback function is called .
 * @param[out]  resp_status, contains the response status (Success or Error code) \n 
 *               0              - SUCCESS \n
 *               Non-Zero Value - ErrorCodes
 * @note        Refer Bluetooth Generic Error Codes section  upto 0x4FF8 from \ref error-codes 
 * @param[out]  bt_event_user_passkey_display, contains the passkey information , please refer \ref rsi_bt_event_user_passkey_display_s structure for more information.
 * @return      void
 * @note      This callback has to be registered using rsi_bt_register_gap_callbacks API
 */
typedef void (*rsi_bt_on_passkey_display_t)(uint16_t resp_status,
                                            rsi_bt_event_user_passkey_display_t *bt_event_user_passkey_display);

/**
 * @typedef  typedef void (*rsi_bt_on_remote_name_request_cancel_t)(uint16_t resp_status,
                                 rsi_bt_event_remote_name_request_cancel_t *remote_name_request_cancel);
 * @brief       When the remote name request cancels the command response received from the module this callback function is called .
 * @param[out]  resp_status, contains the response status (Success or Error code) \n 
 *               0              - SUCCESS \n
 *               Non-Zero Value - ErrorCodes
 * @note        Refer Bluetooth Generic Error Codes section  upto 0x4FF8 from \ref error-codes 
 * @param[out]  remote_name_request_cancel, contains the response to remote name request cancel cmd , please refer \ref rsi_bt_event_remote_name_request_cancel_s structure for more information.
 * @return      void
 * @note        This callback has to be registered using rsi_bt_register_gap_callbacks API
 */
typedef void (*rsi_bt_on_remote_name_request_cancel_t)(
  uint16_t resp_status,
  rsi_bt_event_remote_name_request_cancel_t *remote_name_request_cancel);

/**
 * @typedef    void (*rsi_bt_on_confirm_request_t)(uint16_t resp_status,
 *                                           rsi_bt_event_user_confirmation_request_t *user_confirmation_request);
 * @brief       When the user confirmation request is received from the module this callback is called. \n 
 *              The user has to give rsi_bt_accept_ssp_confirm or rsi_bt_reject_ssp_confirm command upon reception of this event.
 * @param[out]  resp_status, contains the response status (Success or Error code) \n 
 *               0              - SUCCESS \n
 *               Non-Zero Value - ErrorCodes
 * @note        Refer Bluetooth Generic Error Codes section  upto 0x4FF8 from \ref error-codes 
 * @param[out]  user_confirmation_request, contains the confirmation request information ,please refer \ref rsi_bt_event_user_confirmation_request_s structure for more information .

 * @return      void
 * @note        This callback has to be registered using rsi_bt_register_gap_callbacks API
 */
typedef void (*rsi_bt_on_confirm_request_t)(uint16_t resp_status,
                                            rsi_bt_event_user_confirmation_request_t *user_confirmation_request);

/**
 * @typedef void (*rsi_bt_on_pincode_request_t)(uint16_t resp_status,
 *                                           rsi_bt_event_user_pincode_request_t *user_pincode_request);
 * @brief       When pincode request is received from the module, this callback is called. \n 
 *              User has to give rsi_bt_accept_pincode_request command upon reception of this event.
 * @param[out]  resp_status, contains the response status (Success or Error code) \n 
 *               0              - SUCCESS \n
 *               Non-Zero Value - ErrorCodes
 * @note        Refer Bluetooth Generic Error Codes section  upto 0x4FF8 from \ref error-codes 
 * @param[out]  user_pincode_request, contains the pincode request information ,please refer \ref rsi_bt_event_user_pincode_request_s structure for more information.
 * @return      void
 * @note        This callback has to be registered using rsi_bt_register_gap_callbacks API
 */
typedef void (*rsi_bt_on_pincode_request_t)(uint16_t resp_status,
                                            rsi_bt_event_user_pincode_request_t *user_pincode_request);

/**
 * @typedef    void (*rsi_bt_on_passkey_request_t)(uint16_t resp_status,
 *                                           rsi_bt_event_user_passkey_request_t *user_passkey_request);
 * @brief      When the passkey request is received from the module this callback is called. \n 
 *             User has to give rsi_bt_passkey command upon reception of this event.
 * @param[out]  resp_status, contains the response status (Success or Error code) \n 
 *               0              - SUCCESS \n
 *               Non-Zero Value - ErrorCodes
 * @note        Refer Bluetooth Generic Error Codes section  upto 0x4FF8 from \ref error-codes 
 * @param[out]  user_passkey_request, contains the passkey request information ,please refer \ref rsi_bt_event_user_passkey_request_s structure for more information.
 * @return      void
 * @note        This callback has to be registered using rsi_bt_register_gap_callbacks API
 */
typedef void (*rsi_bt_on_passkey_request_t)(uint16_t resp_status,
                                            rsi_bt_event_user_passkey_request_t *user_passkey_request);

/**
 * @typedef    void (*rsi_bt_on_inquiry_complete_t)(uint16_t resp_status);
 * @brief      When inquiry complete status is received from the module this callback is called . \n 
 *             This event will be given by the module when inquiry command is completely executed.
 * @param[out]  resp_status, contains the response status (Success or Error code) \n 
 *               0              - SUCCESS \n
 *               Non-Zero Value - ErrorCodes
 * @note        Refer Bluetooth Generic Error Codes section  upto 0x4FF8 from \ref error-codes 
 * @return      void
 * @note       This callback has to be registered using rsi_bt_register_gap_callbacks API
 */
typedef void (*rsi_bt_on_inquiry_complete_t)(uint16_t resp_status);

/**
 * @typedef    void (*rsi_bt_on_auth_complete_t)(uint16_t resp_status, rsi_bt_event_auth_complete_t *auth_complete);
 * @brief       When authentication complete indication is received from the module this callback is called .
 * @param[out]  resp_status, contains the response status (Success or Error code) \n 
 *               0              - SUCCESS \n
 *               Non-Zero Value - ErrorCodes
 * @note        Refer Bluetooth Generic Error Codes section  upto 0x4FF8 from \ref error-codes 
 * @param[out]  auth_complete, contains the authentication complete information , please refer \ref rsi_bt_event_auth_complete_s structure for more information.
 * @return      void
 * @note        This callback has to be registered using rsi_bt_register_gap_callbacks API
 */
typedef void (*rsi_bt_on_auth_complete_t)(uint16_t resp_status, rsi_bt_event_auth_complete_t *auth_complete);

/**
 * @typedef void (*rsi_bt_on_linkkey_request_t)(uint16_t resp_status,
 *                                           rsi_bt_event_user_linkkey_request_t *user_linkkey_request);
 * @brief       When linkkey request is received from the module this callback is called. \n 
 *              User has to give linkkey reply command upon reception of this event.
 * @param[out]  resp_status, contains the response status (Success or Error code) \n 
 *               0              - SUCCESS \n
 *               Non-Zero Value - ErrorCodes
 * @note        Refer Bluetooth Generic Error Codes section  upto 0x4FF8 from \ref error-codes 
 * @param[out]  user_linkkey_request, contains the linkkey request information , please refer \ref rsi_bt_event_user_linkkey_request_s structure for mre information .
 * @return      void
 * @note        This callback has to be registered using rsi_bt_register_gap_callbacks API
 */
typedef void (*rsi_bt_on_linkkey_request_t)(uint16_t resp_status,
                                            rsi_bt_event_user_linkkey_request_t *user_linkkey_request);

/**
 * @typedef    void (*rsi_bt_on_ssp_complete_t)(uint16_t resp_status, rsi_bt_event_ssp_complete_t *ssp_complete);
 * @brief       When SSP complete status is received from the module this callback is called .
 * @param[out]  resp_status, contains the response status (Success or Error code) \n 
 *               0              - SUCCESS \n
 *               Non-Zero Value - ErrorCodes
 * @note        Refer Bluetooth Generic Error Codes section  upto 0x4FF8 from \ref error-codes 
 * @param[out]  ssp_complete, contains the ssp complete status, please refer \ref rsi_bt_event_ssp_complete_s structure for more information .
 * @return      void
 * @note        This callback has to be registered using rsi_bt_register_gap_callbacks API
 */
typedef void (*rsi_bt_on_ssp_complete_t)(uint16_t resp_status, rsi_bt_event_ssp_complete_t *ssp_complete);

/**
 * @typedef    void (*rsi_bt_on_linkkey_save_t)(uint16_t resp_status, rsi_bt_event_user_linkkey_save_t *user_linkkey_save);
 * @brief       When linkkey save is received from the module this callback is called.
 * @param[out]  resp_status, contains the response status (Success or Error code) \n 
 *               0              - SUCCESS \n
 *               Non-Zero Value - ErrorCodes
 * @note        Refer Bluetooth Generic Error Codes section  upto 0x4FF8 from \ref error-codes 
 * @param[out]  user_linkkey_save, contains the linkkey save information , plesae refer \ref rsi_bt_event_user_linkkey_save_s structure for more information.
 * @return      void
 * @note        This callback has to be registered using rsi_bt_register_gap_callbacks API
 */
typedef void (*rsi_bt_on_linkkey_save_t)(uint16_t resp_status, rsi_bt_event_user_linkkey_save_t *user_linkkey_save);

/**
 * @typedef    void (*rsi_bt_on_get_services_t)(uint16_t resp_status, rsi_bt_resp_query_services_t *service_list);
 * @brief       When the get services command response is received from the module this callback is called .
 * @param[out]  resp_status, contains the response status (Success or Error code) \n 
 *               0              - SUCCESS \n
 *               Non-Zero Value - ErrorCodes
 * @note        Refer Bluetooth Generic Error Codes section  upto 0x4FF8 from \ref error-codes 
 * @param[out]  service_list, contains the response information to get services cmd ,please refer \ref rsi_bt_resp_query_services_s structure for more information.
 * @return      void
 * @note        This callback is Reserved for Future.
 * @note        This callback has to be registered using rsi_bt_register_gap_callbacks API
 */
typedef void (*rsi_bt_on_get_services_t)(uint16_t resp_status, rsi_bt_resp_query_services_t *service_list);

/**
 * @typedef void (*rsi_bt_on_search_service_t)(uint16_t resp_status, uint8_t *status);
 * @brief       When the search service command response is received from the module this calback is called.
 * @param[out]  resp_status, contains the response status (Success or Error code) \n 
 *               0              - SUCCESS \n
 *               Non-Zero Value - ErrorCodes
 * @note        Refer Bluetooth Generic Error Codes section  upto 0x4FF8 from \ref error-codes 
 * @param[out]  status, contains the status response to search service cmd.
 * @return      void
 * @note        This callback is Reserved for Future.
 * @note        This callback has to be registered using rsi_bt_register_gap_callbacks API
 */
typedef void (*rsi_bt_on_search_service_t)(uint16_t resp_status, uint8_t *status);

/**
 * @typedef    void (*rsi_bt_on_mode_chnage_t)(uint16_t resp_status, rsi_bt_event_mode_change_t *mode_change);
 * @brief       When the local device enters / exits the Sniff mode this callback is called.
 * @param[out]  resp_status, contains the response status (Success or Error code) \n 
 *               0              - SUCCESS \n
 *               Non-Zero Value - ErrorCodes
 * @note        Refer Bluetooth Generic Error Codes section  upto 0x4FF8 from \ref error-codes 
 * @param[out]   mode_change, contains the response payload for the mode change response , please refer \ref rsi_bt_event_mode_change_s structure for more information .
 * @return      void
 * @note        This callback has to be registered using rsi_bt_register_gap_callbacks API
 */
typedef void (*rsi_bt_on_mode_chnage_t)(uint16_t resp_status, rsi_bt_event_mode_change_t *mode_change);

/**
 * @typedef     void (*rsi_bt_on_avdtp_stats_t)(uint16_t resp_status, rsi_bt_event_avdtp_stats_t *avdtp_event); 
 * @brief        When AVDTP stats event is receicved from module this callback is called.
 * @param[out]   resp_status, contains the response status (Success or Error code) \n 
 *               0              - SUCCESS \n
 *               Non-Zero Value - ErrorCodes 
 * @param[out]   avdtp_event, contains the avdtp stats  parameters , please refer \ref rsi_bt_event_avdtp_stats_s structure for more information .            
 * @return       void
 * @note         This callback has to be registered using rsi_bt_avdtp_events_register_callbacks API
 */
typedef void (*rsi_bt_on_avdtp_stats_t)(uint16_t resp_status, rsi_bt_event_avdtp_stats_t *avdtp_event);

/**
 * @typedef     void (*rsi_bt_on_ar_stats_t)(uint16_t resp_status, rsi_bt_event_ar_stats_t *ar_event);
 * @brief        When AR stats event is received from module this callback is called.
 * @param[out]   resp_status, contains the response status (Success or Error code) \n 
 *               0              - SUCCESS \n
 *               Non-Zero Value - ErrorCodes
 * @param[out]   ar_event, contains the ar stats  parameters , please refer \ref rsi_bt_event_ar_stats_s structure for more information.
 * @return       void
 * @note         This callback has to be registered using rsi_bt_ar_events_register_callbacks API
 */
typedef void (*rsi_bt_on_ar_stats_t)(uint16_t resp_status, rsi_bt_event_ar_stats_t *ar_event);

/**
 * @typedef     void (*rsi_bt_pkt_change_stats_t)(uint16_t resp_status, rsi_bt_event_pkt_change_t *bt_pkt_change_stats);
 * @brief        When HCI packet type change receicved from module this callback is called.
 * @param[out]   resp_status, contains the response status (Success or Error code) \n 
 *               0              - SUCCESS \n
 *               Non-Zero Value - ErrorCodes
 * @param[out]   avdtp_event, contains the packet type , please refer \ref rsi_bt_event_pkt_change_s structure for more information.            
 * @return       void
 * @note         This callback has to be registered using rsi_bt_pkt_change_events_register_callbacks API
 */

typedef void (*rsi_bt_pkt_change_stats_t)(uint16_t resp_status, rsi_bt_event_pkt_change_t *bt_pkt_change_stats);

/**
 * @typedef void (*rsi_bt_on_sniff_subrating_t)(uint16_t resp_status, rsi_bt_event_sniff_subrating_t *mode_change);
 * @brief       When Sniff subrating is enabled or the parameters are negotiated with the remote device this callback is called .
 * @param[out]  resp_status, contains the response status (Success or Error code) \n 
 *               0              - SUCCESS \n
 *               Non-Zero Value - ErrorCodes
 * @note        Refer Bluetooth Generic Error Codes section  upto 0x4FF8 from \ref error-codes 
 * @param[out]  mode_change, contains parameters related to sniff subrating , please refer \ref rsi_bt_event_sniff_subrating_s structure for more information.
 * @return      void
 * @note        This callback has to be registered using rsi_bt_register_gap_callbacks API
 */
typedef void (*rsi_bt_on_sniff_subrating_t)(uint16_t resp_status, rsi_bt_event_sniff_subrating_t *mode_change);
/**
 * @typedef    void (*rsi_bt_on_connection_initiated_t)(uint16_t resp_status,
 *                                                rsi_bt_event_connection_initiated_t *mode_change);
 * @brief      When the headset initiates the connection this callback is called.
 * @param[out]  resp_status, contains the response status (Success or Error code) \n 
 *               0              - SUCCESS \n
 *               Non-Zero Value - ErrorCodes
 * @note        Refer Bluetooth Generic Error Codes section  upto 0x4FF8 from \ref error-codes 
 * @param[out]  conn_event, if the value is 1 implies connection initiated by remote device , please refer \ref rsi_bt_event_connection_initiated_s structure for more information.
 * @return      void
 * @note       This callback has to be registered using rsi_bt_register_gap_callbacks API
 */
typedef void (*rsi_bt_on_connection_initiated_t)(uint16_t resp_status,
                                                 rsi_bt_event_connection_initiated_t *mode_change);

/** @} */

/**
 * @typedef void (*rsi_bt_on_chip_memory_stats_handler_t)(uint16_t resp_status,
 *                                                     struct chip_bt_buffers_stats_s *chip_bt_buffers_stats);
 * @brief       Callback function to be called when memory stats received from module
 * @note        This API is not supported in the current release.
 * @param[out]   resp_status - contains the response status (Success or Error code)
 * @param[out]   avdtp_event - contains the stats info
 * @return      void
 * @note        Currently Not Using this callback
 */
typedef void (*rsi_bt_on_chip_memory_stats_handler_t)(uint16_t resp_status,
                                                      struct chip_bt_buffers_stats_s *chip_bt_buffers_stats);
/**
 * @typedef    void (*rsi_bt_on_gatt_connection_t)(uint16_t resp_status, rsi_bt_event_gatt_connect_t *gatt_conn);
 * @brief      Callback function to be called if gatt connection response is received
 * @note       This API is not supported in the current release.
 * @param[out]  resp_status, contains the response status (Success or Error code) \n 
 *               0              - SUCCESS \n
 *               Non-Zero Value - ErrorCodes
 * @note        Refer Bluetooth Generic Error Codes section  upto 0x4FF8 from \ref error-codes 
 * @param[out]  gatt_conn, contains the remote device address of gatt connected over classic. \n
 *              Please refer \ref rsi_bt_event_gatt_connect_s structure for more info.
 * @return      void
 */
typedef void (*rsi_bt_on_gatt_connection_t)(uint16_t resp_status, rsi_bt_event_gatt_connect_t *gatt_conn);
/**
 * @typedef void (*rsi_bt_on_gatt_disconnection_t)(uint16_t resp_status, rsi_bt_event_gatt_disconnect_t *gatt_disconn);
 * @brief      Callback function to be called if gatt disconnection response is received
 * @note       This API is not supported in the current release.
 * @param[out]  resp_status, contains the response status (Success or Error code) \n 
 *               0              - SUCCESS \n
 *               Non-Zero Value - ErrorCodes
 * @note        Refer Bluetooth Generic Error Codes section  upto 0x4FF8 from \ref error-codes 
 * @param[out]  gatt_disconn, contains the remote device address of gatt disconnected over classic. \n
 *              Please refer \ref rsi_bt_event_gatt_disconnect_s structure for more info.
 * @return      void
 */
typedef void (*rsi_bt_on_gatt_disconnection_t)(uint16_t resp_status, rsi_bt_event_gatt_disconnect_t *gatt_disconn);

/******************************************************
 * *       BT SPP Response Callbacks Declarations
 * ******************************************************/
/** @addtogroup BT-CLASSIC7
* @{
*/
/**
 * @typedef    void (*rsi_bt_on_spp_connect_t)(uint16_t resp_status, rsi_bt_event_spp_connect_t *spp_connect);
 * @brief       When SPP connected event is raised from the module,this callback will be called .\n 
 *              This event will be given by the module when spp profile level connection happens from either side.
 * @param[out]  resp_status, contains the response status (Success or Error code) \n
 *               0              - SUCCESS \n
 *               Non-Zero Value - ErrorCodes
 * @note        Refer Bluetooth Generic Error Codes section  upto 0x4FF8 from \ref error-codes 
 * @param[out]  spp_connect, contains the spp connect information, please refer \ref rsi_bt_event_spp_connect_s for more information.
 * @return      void
 * @note        This callback has to be registered using rsi_bt_spp_register_callbacks API
 */
typedef void (*rsi_bt_on_spp_connect_t)(uint16_t resp_status, rsi_bt_event_spp_connect_t *spp_connect);

/**
 * @typedef    void (*rsi_bt_on_spp_disconnect_t)(uint16_t resp_status, rsi_bt_event_spp_disconnect_t *spp_disconnect);
 * @brief       When SPP disconnected event is raised from the module,this callback will be called .\n 
 *              This event will be given by the module when spp profile level disconnection happens from either side.
 * @param[out]  resp_status, contains the response status (Success or Error code) \n 
 *               0              - SUCCESS \n
 *               Non-Zero Value - ErrorCodes
 * @note        Refer Bluetooth Generic Error Codes section  upto 0x4FF8 from \ref error-codes 
 * @param[out]  spp_disconnect, contains the spp disconnect information, please refer \ref rsi_bt_event_spp_disconnect_s structure for more information.
 * @return      void
 * @note        This callback has to be registered using rsi_bt_spp_register_callbacks API.
 */
typedef void (*rsi_bt_on_spp_disconnect_t)(uint16_t resp_status, rsi_bt_event_spp_disconnect_t *spp_disconnect);

/**
 * @typedef    void (*rsi_bt_on_spp_rx_data_t)(uint16_t resp_status, rsi_bt_event_spp_receive_t *bt_event_spp_receive);
 * @brief       When SPP receive event is raised from the module,this callback will be called.\n 
 *              This event will be given by the local device when it receives data from the remote device.
 * @param[out]  resp_status, contains the response status (Success or Error code)
 *               0              - SUCCESS \n
 *               Non-Zero Value - ErrorCodes
 * @note        Refer Bluetooth Generic Error Codes section  upto 0x4FF8 from \ref error-codes 
 * @param[out]  bt_event_spp_receive, contains the spp rcvd data information, please refer \ref rsi_bt_event_spp_receive_s structure for more information.
 * @return      void
 * @note        This callback has to be registered using rsi_bt_spp_register_callbacks API
 */
typedef void (*rsi_bt_on_spp_rx_data_t)(uint16_t resp_status, rsi_bt_event_spp_receive_t *bt_event_spp_receive);
/** @} */
/******************************************************
 * *       BT L2CAP Response Callbacks Declarations
 * ******************************************************/

/**
 * @typedef void (*rsi_bt_on_l2cap_connect_t)(uint16_t resp_status, rsi_bt_event_l2cap_connect_t *l2cap_connect);
 * @brief      Callback function to be called if profiles list response is received
 * @param[in]  resp_status, contains the response status (Success or Error code)
 * @param[in]  l2cap_connect, contains the l2cap connect information
 * @return      void
 */
typedef void (*rsi_bt_on_l2cap_connect_t)(uint16_t resp_status, rsi_bt_event_l2cap_connect_t *l2cap_connect);

/**
 * @typedef    void (*rsi_bt_on_l2cap_disconnect_t)(uint16_t resp_status, rsi_bt_event_l2cap_disconnect_t *l2cap_disconnect);
 * @brief      Callback function to be called if profiles list response is received
 * @param[in]  resp_status, contains the response status (Success or Error code)
 * @param[in]  l2cap_disconnect, contains the l2cap disconnect information
 * @return      void
 */
typedef void (*rsi_bt_on_l2cap_disconnect_t)(uint16_t resp_status, rsi_bt_event_l2cap_disconnect_t *l2cap_disconnect);

/**
 * @typedef    void (*rsi_bt_on_l2cap_rx_data_t)(uint16_t resp_status, rsi_bt_event_l2cap_data_t *bt_event_l2cap_receive);
 * @brief      Callback function to be called if profiles list response is received
 * @param[in]  resp_status, contains the response status (Success or Error code)
 * @param[in]  bt_event_hid_receive, contains the hid rcvd data information.
 * @return      void
 */
typedef void (*rsi_bt_on_l2cap_rx_data_t)(uint16_t resp_status, rsi_bt_event_l2cap_data_t *bt_event_l2cap_receive);

/******************************************************
 * *       BT HID Response Callbacks Declarations
 * ******************************************************/

/**
 * @typedef   void (*rsi_bt_on_hid_connect_t)(uint16_t resp_status, rsi_bt_event_hid_connect_t *hid_connect);
 * @brief      Callback function to be called if profiles list response is received
 * @param[in]  resp_status, contains the response status (Success or Error code)
 * @param[in]  hid_connect, contains the hid connect information
 * @return      void
 */
typedef void (*rsi_bt_on_hid_connect_t)(uint16_t resp_status, rsi_bt_event_hid_connect_t *hid_connect);

/**
 * @typedef    void (*rsi_bt_on_hid_disconnect_t)(uint16_t resp_status, rsi_bt_event_hid_disconnect_t *hid_disconnect);
 * @brief      Callback function to be called if profiles list response is received
 * @param[in]  resp_status, contains the response status (Success or Error code)
 * @param[in]  hid_disconnect, contains the hid disconnect information
 * @return      void
 */
typedef void (*rsi_bt_on_hid_disconnect_t)(uint16_t resp_status, rsi_bt_event_hid_disconnect_t *hid_disconnect);

/**
 * @typedef    void (*rsi_bt_on_hid_rx_data_t)(uint16_t resp_status, rsi_bt_event_hid_receive_t *bt_event_hid_receive);
 * @brief      Callback function to be called if profiles list response is received
 * @param[in]  resp_status, contains the response status (Success or Error code)
 * @param[in]  bt_event_hid_receive, contains the hid rcvd data information.
 * @return      void
 * @section description
 */
typedef void (*rsi_bt_on_hid_rx_data_t)(uint16_t resp_status, rsi_bt_event_hid_receive_t *bt_event_hid_receive);

typedef void (*rsi_bt_on_hid_handshake_t)(uint16_t resp_status, rsi_bt_event_hid_receive_t *bt_event_hid_receive);
typedef void (*rsi_bt_on_hid_control_t)(uint16_t resp_status, rsi_bt_event_hid_receive_t *bt_event_hid_receive);
typedef void (*rsi_bt_on_hid_get_report_t)(uint16_t resp_status, rsi_bt_event_hid_receive_t *bt_event_hid_receive);
typedef void (*rsi_bt_on_hid_set_report_t)(uint16_t resp_status, rsi_bt_event_hid_receive_t *bt_event_hid_receive);
typedef void (*rsi_bt_on_hid_get_protocol_t)(uint16_t resp_status, rsi_bt_event_hid_receive_t *bt_event_hid_receive);
typedef void (*rsi_bt_on_hid_set_protocol_t)(uint16_t resp_status, rsi_bt_event_hid_receive_t *bt_event_hid_receive);
typedef void (*rsi_bt_app_iap_conn_t)(uint16_t resp_status, rsi_bt_event_iap_t *iap_disconn);
typedef void (*rsi_bt_app_iap_disconn_t)(uint16_t resp_status, rsi_bt_event_iap_t *iap_disconn);
typedef void (*rsi_bt_app_iap_acc_auth_strt_t)(uint16_t resp_status, rsi_bt_event_iap_t *iap_disconn);
typedef void (*rsi_bt_app_iap_acc_auth_fail_t)(uint16_t resp_status, rsi_bt_event_iap_t *iap_disconn);
typedef void (*rsi_bt_app_iap_acc_auth_complete_t)(uint16_t resp_status, rsi_bt_event_iap_t *iap_disconn);
typedef void (*rsi_bt_app_iap2_auth_start_t)(uint16_t resp_status, rsi_bt_event_iap_t *iap_disconn);
typedef void (*rsi_bt_app_iap2_auth_complete_t)(uint16_t resp_status, rsi_bt_event_iap_t *iap_disconn);
typedef void (*rsi_bt_app_iap2_auth_fail_t)(uint16_t resp_status, rsi_bt_event_iap_t *iap_disconn);
typedef void (*rsi_bt_app_iap2_identification_start_t)(uint16_t resp_status, rsi_bt_event_iap_t *iap_disconn);
typedef void (*rsi_bt_app_iap2_identification_complete_t)(uint16_t resp_status,
                                                          rsi_bt_event_iap2_receive_t *iap_disconn);
typedef void (*rsi_bt_app_iap2_identification_reject_t)(uint16_t resp_status, rsi_bt_event_iap2_receive_t *iap_disconn);
typedef void (*rsi_bt_app_iap2_File_Tx_state_t)(uint16_t resp_status, rsi_bt_event_iap2_receive_t *iap2_receive);
typedef void (*rsi_bt_app_on_iap2_data_rx_t)(uint16_t resp_status, rsi_bt_event_iap2_receive_t *iap2_receive);
//typedef void (*rsi_bt_app_iap2_File_Tx_state_t)(uint16_t resp_status, rsi_bt_event_iap2_receive_t *iap2_receive);
typedef void (*rsi_bt_app_iap2_File_Transfer_rx_data_t)(uint16_t resp_status,
                                                        rsi_bt_event_iap2_receive_t *iap2_receive);

/******************************************************
 * * BT GAP Callbacks register function Declarations
 * ******************************************************/

/*==============================================*/
/**
 * @fn          rsi_bt_register_gap_callbacks 
 * @brief       This function registers the GAP callbacks
 * @param[in]   rsi_bt_on_connect_t                   bt_on_conn_status_event                         : Connection status callback
 * @param[in]   rsi_bt_on_unbond_t                    bt_on_unbond_status                         : Connection status callback
 * @param[in]   rsi_bt_on_disconnect_t                bt_on_disconnect_event                          : Disconnection status callback
 * @param[in]   rsi_bt_on_scan_resp_t                 bt_on_scan_resp_event                           : Scan report callback
 * @param[in]   rsi_bt_on_remote_name_resp_t          bt_on_remote_name_resp_event                    : Remote name report callback
 * @param[in]   rsi_bt_on_passkey_display_t           bt_on_passkey_display_event                     : Passkey display report callback
 * @param[in]   rsi_bt_on_remote_name_request_cancel_t          bt_on_remote_name_request_cancel_event: Remote name request cancel status callback
 * @param[in]   rsi_bt_on_confirm_request_t           bt_on_confirm_request_event                     : Authentication status callback
 * @param[in]   rsi_bt_on_pincode_request_t           bt_on_pincode_request_event                     : Pincode request status callback
 * @param[in]   rsi_bt_on_passkey_request_t           bt_on_passkey_request_event                     : Passkey request status callback
 * @param[in]   rsi_bt_on_inquiry_complete_t          bt_on_inquiry_complete_event                    : Inquiry report callback
 * @param[in]   rsi_bt_on_auth_complete_t             bt_on_auth_complete_event                       : Authentication status callback
 * @param[in]   rsi_bt_on_linkkey_request_t           bt_on_linkkey_request_event                     : Linkkey request report callback
 * @param[in]   rsi_bt_on_ssp_complete_t              bt_on_ssp_complete_event                        : SSP status callback
 * @param[in]   rsi_bt_on_linkkey_save_t              bt_on_linkkey_save_event                        : Linkkey save status callback
 * @param[in]   rsi_bt_on_get_services_t              bt_on_get_services_event                        : Get services report callback
 * @param[in]   rsi_bt_on_search_service_t            bt_on_search_service_event                      : Search service status callback
 * @return     
 *              Non zero - If fails
 *              0 - If success
 *
 *
 */
void rsi_bt_gap_register_callbacks(rsi_bt_on_role_change_t bt_on_role_change_status_event,
                                   rsi_bt_on_connect_t bt_on_conn_status_event,
                                   rsi_bt_on_unbond_t bt_on_unbond_status,
                                   rsi_bt_on_disconnect_t bt_on_disconnect_event,
                                   rsi_bt_on_scan_resp_t bt_on_scan_resp_event,
                                   rsi_bt_on_remote_name_resp_t bt_on_remote_name_resp_event,
                                   rsi_bt_on_passkey_display_t bt_on_passkey_display_event,
                                   rsi_bt_on_remote_name_request_cancel_t bt_on_remote_name_request_cancel_event,
                                   rsi_bt_on_confirm_request_t bt_on_confirm_request_event,
                                   rsi_bt_on_pincode_request_t bt_on_pincode_request_event,
                                   rsi_bt_on_passkey_request_t bt_on_passkey_request_event,
                                   rsi_bt_on_inquiry_complete_t bt_on_inquiry_complete_event,
                                   rsi_bt_on_auth_complete_t bt_on_auth_complete_event,
                                   rsi_bt_on_linkkey_request_t bt_on_linkkey_request_event,
                                   rsi_bt_on_ssp_complete_t bt_on_ssp_complete_event,
                                   rsi_bt_on_linkkey_save_t bt_on_linkkey_save_event,
                                   rsi_bt_on_get_services_t bt_on_get_services_event,
                                   rsi_bt_on_search_service_t bt_on_search_service_event,
                                   rsi_bt_on_mode_chnage_t bt_on_mode_change_event,
                                   rsi_bt_on_sniff_subrating_t bt_on_sniff_subrating_event,
                                   rsi_bt_on_connection_initiated_t bt_on_connection_initiated);

void rsi_bt_iap_register_callbacks(rsi_bt_app_iap_conn_t bt_app_iap_conn,
                                   rsi_bt_app_iap_disconn_t bt_app_iap_disconn,
                                   rsi_bt_app_iap_acc_auth_strt_t bt_app_iap_acc_auth_strt,
                                   rsi_bt_app_iap_acc_auth_fail_t bt_app_iap_acc_auth_fail,
                                   rsi_bt_app_iap_acc_auth_complete_t bt_app_iap_acc_auth_complete,
                                   rsi_bt_app_iap2_auth_start_t bt_app_iap2_auth_start,
                                   rsi_bt_app_iap2_auth_complete_t bt_app_iap2_auth_complete,
                                   rsi_bt_app_iap2_auth_fail_t bt_app_iap2_auth_fail,
                                   rsi_bt_app_iap2_identification_start_t bt_app_iap2_identification_start,
                                   rsi_bt_app_iap2_identification_complete_t bt_app_iap2_identification_complete,
                                   rsi_bt_app_iap2_identification_reject_t bt_app_iap2_identification_reject,
                                   rsi_bt_app_on_iap2_data_rx_t bt_app_on_iap2_data_rx,
                                   rsi_bt_app_iap2_File_Tx_state_t bt_app_iap2_File_Transfer_state,
                                   rsi_bt_app_iap2_File_Transfer_rx_data_t bt_app_iap2_File_Transfer_data);

/**
 * @fn          rsi_bt_gatt_extended_register_callbacks 
 *
 *
 */

void rsi_bt_gatt_extended_register_callbacks(rsi_bt_on_gatt_connection_t bt_on_gatt_connection_event,
                                             rsi_bt_on_gatt_disconnection_t bt_on_gatt_disconnection_event);

/**
 * @fn          rsi_bt_avdtp_events_register_callbacks 
 *
 */

void rsi_bt_avdtp_events_register_callbacks(rsi_bt_on_avdtp_stats_t bt_on_avdtp_stats_event);

/**
 * @fn          rsi_bt_ar_events_register_callbacks
 * @brief       This function registers the AR Event callbacks
 * @param[in]   rsi_bt_on_avdtp_stats_t               bt_on_ar_stats_event
 * @return
 *              Non zero - If fails
 *              0 - If success
 *
 * @section description
 * This function registers the ar stats callbacks
 *
 */

void rsi_bt_ar_events_register_callbacks(rsi_bt_on_ar_stats_t bt_on_ar_stats_event);

/**
 * @fn          rsi_bt_pkt_change_events_register_callbacks
 *
 */
void rsi_bt_pkt_change_events_register_callbacks(rsi_bt_pkt_change_stats_t bt_pkt_change_stats_event);
/**
 * @fn          rsi_bt_on_chip_memory_status_callbacks_register
 *
 */
void rsi_bt_on_chip_memory_status_callbacks_register(
  rsi_bt_on_chip_memory_stats_handler_t bt_on_chip_memory_stats_event);
/******************************************************
 * * BT SPP Callbacks register function Declarations
 * ******************************************************/

/*==============================================*/

/**
 * @fn          rsi_bt_spp_register_callbacks 
 *
 */
void rsi_bt_spp_register_callbacks(rsi_bt_on_spp_connect_t bt_on_spp_connect_event,
                                   rsi_bt_on_spp_disconnect_t bt_on_spp_disconnect_event,
                                   rsi_bt_on_spp_rx_data_t bt_on_spp_rx_data_event);

void rsi_bt_l2cap_register_callbacks(rsi_bt_on_l2cap_connect_t bt_on_l2cap_connect_event,
                                     rsi_bt_on_l2cap_rx_data_t bt_on_l2cap_rx_data_event);
/*==============================================*/
/**
 * @fn         rsi_bt_set_local_class_of_device
 */
int32_t rsi_bt_set_local_class_of_device(uint32_t class_of_device);

/*==============================================*/
/**
 * @fn         rsi_bt_get_local_class_of_device
 */
int32_t rsi_bt_get_local_class_of_device(uint8_t *resp);

/*==============================================*/
/**
 * @fn         rsi_bt_start_discoverable
 */
int32_t rsi_bt_start_discoverable(void);

/*==============================================*/
/**
 * @fn         rsi_bt_start_limited_discoverable
 */
int32_t rsi_bt_start_limited_discoverable(int32_t time_out_ms);

/*==============================================*/
/**
 * @fn         rsi_bt_stop_discoverable
 */
int32_t rsi_bt_stop_discoverable(void);

/*==============================================*/
/**
 * @fn         rsi_bt_get_discoverable_status
 */
int32_t rsi_bt_get_discoverable_status(uint8_t *resp);

/*==============================================*/
/**
 * @fn         rsi_bt_set_connectable
 */
int32_t rsi_bt_set_connectable(void);

/*==============================================*/
/**
 * @fn         rsi_bt_set_non_connectable
 */
int32_t rsi_bt_set_non_connectable(void);

/*==============================================*/
/**
 * @fn         rsi_bt_get_connectable_status
 */
int32_t rsi_bt_get_connectable_status(uint8_t *resp);

/*==============================================*/
/**
 * @fn         rsi_bt_enable_authentication
 */
int32_t rsi_bt_enable_authentication(void);

/*==============================================*/
/**
 * @fn         rsi_bt_disable_authentication
 */
int32_t rsi_bt_disable_authentication(void);

/*==============================================*/
/**
 * @fn         rsi_bt_get_authentication
 */
int32_t rsi_bt_get_authentication(uint8_t *resp);

/*==============================================*/
/**
 * @fn         rsi_bt_remote_name_request_async
 */
int32_t rsi_bt_remote_name_request_async(uint8_t *remote_dev_addr,
                                         rsi_bt_event_remote_device_name_t *bt_event_remote_device_name);

/*==============================================*/
/**
 * @fn         rsi_bt_remote_name_request_cancel
 */
int32_t rsi_bt_remote_name_request_cancel(uint8_t *remote_dev_addr);

/*==============================================*/
/**
 * @fn         rsi_bt_spp_connect
 * */
int32_t rsi_bt_spp_connect(uint8_t *remote_dev_addr);

/*==============================================*/
/**

 * @fn         rsi_ble_set_eir_data
 *
 */
int32_t rsi_bt_set_eir_data(uint8_t *data, uint16_t data_len);

/*==============================================*/
/**
 * @fn         rsi_bt_spp_init
 */
int32_t rsi_bt_spp_init(void);

/*==============================================*/
/**
 * @fn         rsi_bt_spp_transfer
 *
 */
int32_t rsi_bt_spp_transfer(uint8_t *remote_dev_addr, uint8_t *data, uint16_t length);

/*==============================================*/
/**
 * @fn         rsi_bt_inquiry
 */
int32_t rsi_bt_inquiry(uint8_t inquiry_type, uint32_t inquiry_duration, uint8_t max_devices);

/*==============================================*/
/**
 * @fn         rsi_bt_cancel_inquiry
 */
int32_t rsi_bt_cancel_inquiry(void);

/*==============================================*/
/**
 * @fn         rsi_bt_connect
 */
int32_t rsi_bt_connect(uint8_t *remote_dev_addr);

/*==============================================*/
/**
 * @fn         rsi_bt_cancel_connect
 *
 */
int32_t rsi_bt_cancel_connect(uint8_t *remote_dev_address);

/*==============================================*/
/**
 * @fn         rsi_bt_disconnect
 *
 */
int32_t rsi_bt_disconnect(uint8_t *remote_dev_address);

/*==============================================*/
/**
 * @fn         rsi_bt_accept_ssp_confirm
 *
 */
int32_t rsi_bt_accept_ssp_confirm(uint8_t *remote_dev_address);

/*==============================================*/
/**
 * @fn         rsi_bt_reject_ssp_confirm
 *
 */
int32_t rsi_bt_reject_ssp_confirm(uint8_t *remote_dev_address);

/*==============================================*/
/**
 * @fn         rsi_bt_passkey
 *
 */
int32_t rsi_bt_passkey(uint8_t *remote_dev_addr, uint32_t passkey, uint8_t reply_type);

/*==============================================*/
/**
 * @fn         rsi_bt_pincode_request_reply
 *
 */
int32_t rsi_bt_pincode_request_reply(uint8_t *remote_dev_addr, uint8_t *pin_code, uint8_t reply_type);

/*==============================================*/
/**
 * @fn         rsi_bt_linkkey_request_reply
 *
 */
int32_t rsi_bt_linkkey_request_reply(uint8_t *remote_dev_addr, uint8_t *linkkey, uint8_t reply_type);

/*==============================================*/
/**
 * @fn         rsi_bt_get_local_device_role
 *
 */
int32_t rsi_bt_get_local_device_role(uint8_t *remote_dev_addr, uint8_t *resp);

/*==============================================*/
/**
 * @fn         rsi_bt_get_services_async
 *
 */
int32_t rsi_bt_get_services_async(uint8_t *remote_dev_addr, rsi_bt_resp_query_services_t *bt_resp_query_services);

/*==============================================*/
/**
 * @fn         rsi_bt_search_service_async
 *
 */
int32_t rsi_bt_search_service_async(uint8_t *remote_dev_addr, uint32_t service_uuid);

/*==============================================*/
/**
 * @fn         rsi_bt_sniff_mode
 */
int32_t rsi_bt_sniff_mode(uint8_t *remote_dev_addr,
                          uint16_t sniff_max_intv,
                          uint16_t sniff_min_intv,
                          uint16_t sniff_attempt,
                          uint16_t sniff_tout);

/*==============================================*/
/**
 * @fn         rsi_bt_sniff_exit_mode
 */
int32_t rsi_bt_sniff_exit_mode(uint8_t *remote_dev_addr);

/*==============================================*/
/**
 * @fn         rsi_bt_sniff_subrating_mode
 */
int32_t rsi_bt_sniff_subrating_mode(uint8_t *remote_dev_addr,
                                    uint16_t max_latency,
                                    uint16_t min_remote_tout,
                                    uint16_t min_local_tout);

/*==============================================*/
/**
 * @fn         rsi_bt_set_ssp_mode
 */
int32_t rsi_bt_set_ssp_mode(uint8_t pair_mode, uint8_t IOcapability);
int32_t rsi_bt_iap_conn(uint8_t *remote_dev_addr, uint8_t version);
int32_t rsi_bt_iap_disconn(uint8_t *remote_dev_addr, uint8_t version);
int32_t rsi_bt_iap_set_accessory_info(uint8_t type, uint8_t length, uint8_t *data);
int32_t rsi_bt_iap_set_voice_over(uint8_t mode, uint8_t restore);
int32_t rsi_bt_iap1_identification(void);
int32_t rsi_bt_iap2_identification(void);
int32_t rsi_bt_iap1_device_authentication(void);
int32_t rsi_bt_iap_init(void);
int32_t rsi_bt_iap_set_protocol_type(uint8_t type);
int32_t rsi_bt_iap_find_protocol_type(uint8_t *resp);

/* A2DP Profile */
typedef struct rsi_bt_event_a2dp_connect_s {
  /** remote device address*/
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
} rsi_bt_event_a2dp_connect_t;

typedef struct rsi_bt_event_a2dp_disconnect_s {
  /** remote device address*/
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
} rsi_bt_event_a2dp_disconnect_t;

typedef struct rsi_bt_event_a2dp_configure_s {
  /** remote device address*/
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
} rsi_bt_event_a2dp_configure_t;

typedef struct rsi_bt_event_a2dp_start_s {
  /** remote device address*/
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];

  /** Sampling frequency of the A2DP Stream codec*/
  uint16_t sample_freq;

  /** MTU size supported by the remote device*/
  uint16_t rem_mtu_size;
#if BT_HFP_HF_ROLE
  uint16_t rx_mtu_size;
#endif
} rsi_bt_event_a2dp_start_t;

typedef struct rsi_bt_event_a2dp_open_s {
  /** remote device address*/
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
} rsi_bt_event_a2dp_open_t;

typedef struct rsi_bt_event_a2dp_suspend_s {
  /** remote device address*/
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
} rsi_bt_event_a2dp_suspend_t;

typedef struct rsi_bt_event_a2dp_abort_s {
  /** remote device address*/
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
} rsi_bt_event_a2dp_abort_t;

typedef struct rsi_bt_event_a2dp_close_s {
  /** remote device address*/
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
} rsi_bt_event_a2dp_close_t;

typedef struct rsi_bt_event_a2dp_encode_data_s {
  /** remote device address*/
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];

  /** Length of the received encoded data*/
  uint16_t encode_data_len;

  /** Buffer holding the received a2dp encoded data. It holds the max payload length of 1k bytes*/
  uint8_t encode_data[RSI_BT_MAX_PAYLOAD_SIZE];
} rsi_bt_event_a2dp_encode_data_t;

typedef struct rsi_bt_event_a2dp_pcm_data_s {
  /** remote device address*/
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];

  /** Length of the received pcm data*/
  uint16_t pcm_data_len;

  /** Buffer holding the received a2dp pcm data. It holds the max payload length of 1k bytes*/
  uint8_t pcm_data[RSI_BT_MAX_PAYLOAD_SIZE];
} rsi_bt_event_a2dp_pcm_data_t;

typedef struct rsi_bt_event_a2dp_more_data_req_s {
  /** remote device address*/
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];

  /** Number of Bytes to be send from Host to Stack*/
  uint16_t NbrBytesReqd;
} rsi_bt_event_a2dp_more_data_req_t;

typedef struct rsi_bt_event_a2dp_reconfig_s {
  /** remote device address*/
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];

  /** If Reconfig Command given to Remote Device and there is no response till timer exits, \n
  then ERR_BT_A2DP_RECONFIG_CMD_TIMEOUT 0x4514 will be send through this status from stack to host.*/
  uint16_t resp_status;
} rsi_bt_event_a2dp_reconfig_t;
/******************************************************
 * *       BT A2DP Response Callbacks Declarations
 * ******************************************************/
/** @addtogroup BT-CLASSIC7
* @{
*/
/**
 * @typedef    void (*rsi_bt_on_a2dp_connect_t)(uint16_t resp_status, rsi_bt_event_a2dp_connect_t *a2dp_connect);
 * @brief      When A2DP connected event is raised from the module, this callback will be called. \n 
 *             This event will be given by the module when A2DP profile level connection happens from either side.
 * @param[out] resp_status, contains the response status (Success or Error code) \n
 *             0              - SUCCESS \n 
 *             Non-Zero Value - ErrorCodes 
 * @note       Refer Bluetooth Generic Error Codes section upto 0x4FF8 from \ref error-codes .
 * @param[out] a2dp_connect, contains the a2dp connect information, please refer \ref rsi_bt_event_a2dp_connect_s structure \n 
 *             for more info. 
 * @return      void
 * @note       This callback has to be registered using rsi_bt_a2dp_register_callbacks API
 */
typedef void (*rsi_bt_on_a2dp_connect_t)(uint16_t resp_status, rsi_bt_event_a2dp_connect_t *a2dp_connect);

/**
 * @typedef    void (*rsi_bt_on_a2dp_disconnect_t)(uint16_t resp_status, rsi_bt_event_a2dp_disconnect_t *a2dp_disconnect);
 * @brief      When A2DP disconnected event is raised from the module, this callback will be called. \n 
 *             This event will be given by the module when a2dp profile level disconnection happens from either side.
 * @param[out] resp_status, contains the response status (Success or Error code) \n
 *             0              - SUCCESS \n 
 *             Non-Zero Value - ErrorCodes 
 * @note       Refer Bluetooth Generic Error Codes section upto 0x4FF8 from \ref error-codes .
 * @param[out] a2dp_disconnect, contains the a2dp disconnect information, please refer \ref rsi_bt_event_a2dp_disconnect_s structure \n 
 *             for more info.
 * @return      void
 * @note       This callback has to be registered using rsi_bt_a2dp_register_callbacks API
 */
typedef void (*rsi_bt_on_a2dp_disconnect_t)(uint16_t resp_status, rsi_bt_event_a2dp_disconnect_t *a2dp_disconnect);

/**
 * @typedef    void (*rsi_bt_on_a2dp_configure_t)(uint16_t resp_status, rsi_bt_event_a2dp_configure_t *a2dp_configure);
 * @brief      When A2DP configured event is raised from the module, this callback will be called. \n 
 *             This event will be given by the module when a2dp profile onfiguration happens from either side.
 * @param[out] resp_status, contains the response status (Success or Error code) \n
 *             0              - SUCCESS \n 
 *             Non-Zero Value - ErrorCodes 
 * @note       Refer Bluetooth Generic Error Codes section upto 0x4FF8 from \ref error-codes .
 * @param[out] a2dp_configure, contains the a2dp configure information, please refer \ref rsi_bt_event_a2dp_configure_s structure \n 
 *             for more info.
 * @return      void
 * @note       This callback has to be registered using rsi_bt_a2dp_register_callbacks API
 */
typedef void (*rsi_bt_on_a2dp_configure_t)(uint16_t resp_status, rsi_bt_event_a2dp_configure_t *a2dp_configure);

/**
 * @typedef    void (*rsi_bt_on_a2dp_open_t)(uint16_t resp_status, rsi_bt_event_a2dp_open_t *a2dp_open);
 * @brief      When A2DP open event is raised from the module, this callback will be called. \n 
 *             This event will be given by the module when a2dp opens in either side.
 * @param[out] resp_status, contains the response status (Success or Error code) \n 
 *             0              - SUCCESS \n 
 *             Non-Zero Value - ErrorCodes 
 * @note       Refer Bluetooth Generic Error Codes section upto 0x4FF8 from \ref error-codes .
 * @param[out] a2dp_open, contains the a2dp open information, please refer \ref rsi_bt_event_a2dp_open_s structure \n 
 *             for more info.
 * @return      void
 * @note       This callback has to be registered using rsi_bt_a2dp_register_callbacks API
 */
typedef void (*rsi_bt_on_a2dp_open_t)(uint16_t resp_status, rsi_bt_event_a2dp_open_t *a2dp_open);

/**
 * @typedef    void (*rsi_bt_on_a2dp_start_t)(uint16_t resp_status, rsi_bt_event_a2dp_start_t *a2dp_start);
 * @brief      When A2DP start event is raised from the module, this callback is called. \n 
 *             This event will be given by the module when a2dp starts from either side.
 * @param[out] resp_status, contains the response status (Success or Error code) \n 
 *             0              - SUCCESS \n 
 *             Non-Zero Value - ErrorCodes 
 * @note       Refer Bluetooth Generic Error Codes section upto 0x4FF8 from \ref error-codes .
 * @param[out] a2dp_start, contains the a2dp start information, please refer \ref rsi_bt_event_a2dp_start_s structure \n 
 *             for more info.
 * @return      void
 * @note       This callback has to be registered using rsi_bt_a2dp_register_callbacks API
 */
typedef void (*rsi_bt_on_a2dp_start_t)(uint16_t resp_status, rsi_bt_event_a2dp_start_t *a2dp_start);

/**
 * @typedef    void (*rsi_bt_on_a2dp_suspend_t)(uint16_t resp_status, rsi_bt_event_a2dp_suspend_t *a2dp_suspend);
 * @brief      When A2DP suspend event is raised from the module, this callback will be called. \n 
 *             This event will be given by the module when a2dp suspend happens from either side.
 * @param[out] resp_status, contains the response status (Success or Error code) \n
 *             0              - SUCCESS \n 
 *             Non-Zero Value - ErrorCodes 
 * @note       Refer Bluetooth Generic Error Codes section upto 0x4FF8 from \ref error-codes .
 * @param[out] a2dp suspend, contains the a2dp suspend information, please refer \ref rsi_bt_event_a2dp_suspend_s structure \n 
 *             for more info.
 * @return      void
 * @note       This callback has to be registered using rsi_bt_a2dp_register_callbacks API
 */
typedef void (*rsi_bt_on_a2dp_suspend_t)(uint16_t resp_status, rsi_bt_event_a2dp_suspend_t *a2dp_suspend);

/**
 * @typedef    void (*rsi_bt_on_a2dp_abort_t)(uint16_t resp_status, rsi_bt_event_a2dp_abort_t *a2dp_abort);
 * @brief      When A2DP abort event is raised from the module, this callback will be called. \n
 *             This event will be given by the module when a2dp abort happens from either side.
 * @param[out] resp_status, contains the response status (Success or Error code) \n 
 *             0              - SUCCESS \n 
 *             Non-Zero Value - ErrorCodes 
 * @note       Refer Bluetooth Generic Error Codes section upto 0x4FF8 from \ref error-codes .
 * @param[out] a2dp_abort, contains the a2dp abort information, please refer \ref rsi_bt_event_a2dp_abort_s structure \n 
 *             for more info. 
 * @return      void
 * @note       This callback has to be registered using rsi_bt_a2dp_register_callbacks API
 */
typedef void (*rsi_bt_on_a2dp_abort_t)(uint16_t resp_status, rsi_bt_event_a2dp_abort_t *a2dp_abort);

/**
 * @typedef    void (*rsi_bt_on_a2dp_close_t)(uint16_t resp_status, rsi_bt_event_a2dp_close_t *a2dp_close);
 * @brief      When A2DP close event is raised from the module, this callback will be called. \n
 *             This event will be given by the module when a2dp closed from either side.
 * @param[out] resp_status, contains the response status (Success or Error code) \n 
 *             0              - SUCCESS \n 
 *             Non-Zero Value - ErrorCodes 
 * @note       Refer Bluetooth Generic Error Codes section upto 0x4FF8 from \ref error-codes .
 * @param[out] a2dp_close, contains the a2dp close information, please refer \ref rsi_bt_event_a2dp_close_s structure \n 
 *             for more info. 
 * @return      void
 * @note       This callback has to be registered using rsi_bt_a2dp_register_callbacks API
 */
typedef void (*rsi_bt_on_a2dp_close_t)(uint16_t resp_status, rsi_bt_event_a2dp_close_t *a2dp_close);

/**
 * @typedef    void (*rsi_bt_on_a2dp_encode_data_t)(uint16_t resp_status, rsi_bt_event_a2dp_encode_data_t *a2dp_encode_data);
 * @brief      Callback function to be called if SBC data received from TA
 * @param[out] resp_status, contains the response status (Success or Error code) \n 
 *             0              - SUCCESS \n 
 *             Non-Zero Value - ErrorCodes 
 * @note       Refer Bluetooth Generic Error Codes section upto 0x4FF8 from \ref error-codes .
 * @param[out] a2dp_encode_data, contains the SBC encoded packet information, please refer \ref rsi_bt_event_a2dp_encode_data_s \n
 *             structure for more info.
 * @return      void
 * @note       This callback has to be registered using rsi_bt_a2dp_register_callbacks API
 */
typedef void (*rsi_bt_on_a2dp_encode_data_t)(uint16_t resp_status, rsi_bt_event_a2dp_encode_data_t *a2dp_encode_data);

/**
 * @typedef    void (*rsi_bt_on_a2dp_pcm_data_t)(uint16_t resp_status, rsi_bt_event_a2dp_pcm_data_t *a2dp_pcm_data);
 * @brief      Callback function to be called if PCM data is received from TA
 * @param[out] resp_status, contains the response status (Success or Error code) \n 
 *             0              - SUCCESS \n 
 *             Non-Zero Value - ErrorCodes 
 * @note       Refer Bluetooth Generic Error Codes section upto 0x4FF8 from \ref error-codes .
 * @param[out] a2dp_pcm_data, contains the PCM audio data information
 * @return      void
 * @note       This callback has to be registered using rsi_bt_a2dp_register_callbacks API
 */
typedef void (*rsi_bt_on_a2dp_pcm_data_t)(uint16_t resp_status, rsi_bt_event_a2dp_pcm_data_t *a2dp_pcm_data);

/**
 * @typedef    void (*rsi_bt_on_a2dp_data_req_t)(uint16_t resp_status, rsi_bt_event_a2dp_more_data_req_t *a2dp_more_data_req);
 * @brief      Callback function to be called if more data request evenis received
 * @param[out] resp_status, contains the response status (Success or Error code) \n 
 *             0              - SUCCESS \n 
 *             Non-Zero Value - ErrorCodes 
 * @note       Refer Bluetooth Generic Error Codes section upto 0x4FF8 from \ref error-codes .
 * @param[out] a2dp_more_data_req, contains the a2dp_more_data_req information
 * @return      void
 * @note       This callback has to be registered using rsi_bt_a2dp_register_callbacks API
 */
typedef void (*rsi_bt_on_a2dp_data_req_t)(uint16_t resp_status, rsi_bt_event_a2dp_more_data_req_t *a2dp_more_data_req);

/**
 * @typedef    void (*rsi_bt_on_a2dp_reconfig_t)(uint16_t resp_status, rsi_bt_event_a2dp_reconfig_t *a2dp_reconfig);
 * @brief      When A2DP Reconfig event is raised from the module, this callback will be called.
 * @param[out] resp_status, contains the response status (Success or Error code) \n 
 *             0              - SUCCESS \n 
 *             Non-Zero Value - ErrorCodes 
 * @note       Refer Bluetooth Generic Error Codes section upto 0x4FF8 from \ref error-codes .
 * @param[out] a2dp_reconfig, contains the a2dp reconfig information, please refer \ref rsi_bt_event_a2dp_reconfig_s structure for more info
 * @return      void
 * @note       This callback has to be registered using rsi_bt_a2dp_register_callbacks API
 */
typedef void (*rsi_bt_on_a2dp_reconfig_t)(uint16_t resp_status, rsi_bt_event_a2dp_reconfig_t *a2dp_reconfig);
/** @} */
/******************************************************
 * * BT A2DP Callbacks register function Declarations
 * ******************************************************/

/*==============================================*/

/**
 * @fn          rsi_bt_spp_register_callbacks 
 * @brief       This function registers the SPP callbacks
 * @param[in]   rsi_bt_on_a2dp_connect_t         bt_on_a2dp_connect_event              : a2dp connection status callback
 * @param[in]   rsi_bt_on_a2dp_disconnect_t      bt_on_a2dp_disconnect_event           : a2dp disconnection status callback
 * @param[in]   rsi_bt_on_a2dp_configure_t       bt_on_a2dp_configure_event            : a2dp configure status callback
 * @param[in]   rsi_bt_on_a2dp_open_t            bt_on_a2dp_open_event                 : a2dp open status callback
 * @param[in]   rsi_bt_on_a2dp_start_t           bt_on_a2dp_start_event                : a2dp start status callback
 * @param[in]   rsi_bt_on_a2dp_suspend_t         bt_on_a2dp_suspend_event              : a2dp suspend status callback
 * @param[in]   rsi_bt_on_a2dp_abort_t           bt_on_a2dp_abort_event                : a2dp abort status callback
 * @param[in]   rsi_bt_on_a2dp_close_t           bt_on_a2dp_close_event                : a2dp close status callback
 * @param[in]   rsi_bt_on_a2dp_encode_data_t     bt_on_a2dp_encode_data_event          : a2dp SBC packet received callback
 * @param[in]   rsi_bt_on_a2dp_pcm_data_t        bt_on_a2dp_pcm_data_event             : a2dp PCM packet received callback
 * @param[in]   rsi_bt_on_a2dp_data_req_t        rsi_bt_on_a2dp_data_req_t             : a2dp data request received callback
 * @param[in]   rsi_bt_on_a2dp_reconfig_t        bt_on_a2dp_reconfig_event             : a2dp reconfig status callback
 * @return     
 *              Non zero - If fails
 *              0 - If success
 *
 *
 */
void rsi_bt_a2dp_register_callbacks(rsi_bt_on_a2dp_connect_t bt_on_a2dp_connect_event,
                                    rsi_bt_on_a2dp_disconnect_t bt_on_a2dp_disconnect_event,
                                    rsi_bt_on_a2dp_configure_t bt_on_a2dp_configure_event,
                                    rsi_bt_on_a2dp_open_t bt_on_a2dp_open_event,
                                    rsi_bt_on_a2dp_start_t bt_on_a2dp_start_event,
                                    rsi_bt_on_a2dp_suspend_t bt_on_a2dp_suspend_event,
                                    rsi_bt_on_a2dp_abort_t bt_on_a2dp_abort_event,
                                    rsi_bt_on_a2dp_close_t bt_on_a2dp_close_event,
                                    rsi_bt_on_a2dp_encode_data_t bt_on_a2dp_encode_data_event,
                                    rsi_bt_on_a2dp_pcm_data_t bt_on_a2dp_pcm_data_event,
                                    rsi_bt_on_a2dp_data_req_t bt_on_a2dp_data_req_t,
                                    rsi_bt_on_a2dp_reconfig_t bt_on_a2dp_reconfig_event);

/* AVRCP Profile */
typedef struct rsi_bt_event_avrcp_connect_s {

  /** remote device address*/
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
} rsi_bt_event_avrcp_connect_t;

typedef struct rsi_bt_event_avrcp_disconnect_s {

  /** remote device address*/
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
} rsi_bt_event_avrcp_disconnect_t;

typedef struct rsi_bt_event_avrcp_play_s {

  /** remote device address*/
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];

  /** Status of the Button Pressed/Released
  0 - BUTTON_PUSHED, 1 - BUTTON_RELEASED*/
  uint8_t status_flag;
} rsi_bt_event_avrcp_play_t;

typedef struct rsi_bt_event_avrcp_pause_s {

  /** remote device address*/
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];

  /** Status of the Button Pressed/Released
  0 - BUTTON_PUSHED, 1 - BUTTON_RELEASED*/
  uint8_t status_flag;
} rsi_bt_event_avrcp_pause_t;

typedef struct rsi_bt_event_avrcp_stop_s {

  /** remote device address*/
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];

  /** Status of the Button Pressed/Released
  0 - BUTTON_PUSHED, 1 - BUTTON_RELEASED*/
  uint8_t status_flag;
} rsi_bt_event_avrcp_stop_t;

typedef struct rsi_bt_event_avrcp_next_s {

  /** remote device address*/
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];

  /** Status of the Button Pressed/Released
  0 - BUTTON_PUSHED, 1 - BUTTON_RELEASED*/
  uint8_t status_flag;
} rsi_bt_event_avrcp_next_t;

typedef struct rsi_bt_event_avrcp_previous_s {

  /** remote device address*/
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];

  /** Status of the Button Pressed/Released
  0 - BUTTON_PUSHED, 1 - BUTTON_RELEASED*/
  uint8_t status_flag;
} rsi_bt_event_avrcp_previous_t;

typedef struct rsi_bt_event_avrcp_vol_up_s {

  /** remote device address*/
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];

  /** Status of the Button Pressed/Released
  0 - BUTTON_PUSHED, 1 - BUTTON_RELEASED*/
  uint8_t status_flag;
} rsi_bt_event_avrcp_vol_up_t;

typedef struct rsi_bt_event_avrcp_vol_down_s {

  /** remote device address*/
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];

  /** Status of the Button Pressed/Released
  0 - BUTTON_PUSHED, 1 - BUTTON_RELEASED*/
  uint8_t status_flag;
} rsi_bt_event_avrcp_vol_down_t;

typedef struct rsi_bt_event_avrcp_get_elem_attr_resp_s {
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
  uint8_t pkt_type;
  uint16_t param_len;
  uint8_t param[512];
} rsi_bt_event_avrcp_get_elem_attr_resp_t;

typedef struct player_att_value_s {

  /** Attribute ID*/
  uint8_t attid;

  /** Attribute Value*/
  uint8_t attvalue;

  /** Reserved For Future Use*/
  uint8_t reserved[2];
} player_att_value_t;

typedef struct player_att_val_list_s {
  uint8_t nbr_att;
  player_att_value_t att_list[5];
} player_att_val_list_t;

typedef struct att_val_s {
  /** Player application setting attribute ID for which the value is returned */
  uint8_t att_id;

  /** Currently set Player Application Setting value on the TG for \n 
  the above PlayerApplicationSettingA ttributeID.*/
  uint8_t att_val;

  /** Reserved for Future Purpose*/
  uint8_t reserved[2];
} att_val_t;
#define MAX_ATT_VALS 5
typedef struct att_val_list_s {
  /** Please refer \ref att_val_s structure for more info*/
  att_val_t att_vals[MAX_ATT_VALS];

  /** Number of player application setting attributes */
  uint8_t nbr_atts;
} att_val_list_t;

typedef union notify_val_s {
  /** Play Status \n 
  0 - STOPPED\n
  1 - PLAYING\n
  2 - PAUSED\n
  3 - FWD_SEEK\n
  4 - REV_SEEK\n
  FF - ERROR*/
  uint8_t player_status;

  /** Current Playing Track Index*/
  uint32_t track_num;

  /** Current playback position in millisecond */
  uint32_t track_pos;

  /** Battery Status \n
  0 - NORMAL \n
  1 - WARNING \n 
  2 - CRITICAL \n 
  3 - EXTERNAL \n 
  4 - FULLCHARGE */
  uint8_t battery_status;

  /** System Status \n
  0 - POWER_ON \n 
  1 - POWER_OFF \n 
  2 - UNPLUGGED */
  uint8_t system_status;

  /** Absolute Volume Value*/
  uint8_t abs_vol;

  /** Media Player ID*/
  uint16_t playerid;

  /** Unique ID for the MediaPlayer*/
  uint16_t uidcounter;

  /** Please refer \ref att_val_list_s structure for more info*/
  att_val_list_t att_val_list;
} notify_val_t;

typedef struct avrcp_notify_s {

  /** ID's for events received via Avrcp Registration Notification \n
  #define AVRCP_EVENT_PLAYBACK_STATUS_CHANGED            0x01 \n
  #define AVRCP_EVENT_TRACK_CHANGED                      0x02 \n
  #define AVRCP_EVENT_TRACK_REACHED_END                  0x03 \n
  #define AVRCP_EVENT_TRACK_REACHED_START                0x04 \n
  #define AVRCP_EVENT_PLAYBACK_POS_CHANGED               0x05 \n
  #define AVRCP_EVENT_BATT_STATUS_CHANGED                0x06 \n
  #define AVRCP_EVENT_SYSTEM_STATUS_CHANGED              0x07 \n
  #define AVRCP_EVENT_PLAYER_APPLICATION_SETTING_CHANGED 0x08 \n
  #define AVRCP_EVENT_NOW_PLAYING_CONTENT_CHANGED        0x09 \n
  #define AVRCP_EVENT_AVAILABLE_PLAYERS_CHANGED          0x0a \n
  #define AVRCP_EVENT_ADDRESSED_PLAYER_CHANGED           0x0b \n
  #define AVRCP_EVENT_UIDS_CHANGED                       0x0c \n
  #define AVRCP_EVENT_VOLUME_CHANGED                     0x0d*/
  uint8_t eventid;

  uint8_t rtype;
  uint8_t Reserved[2];
  /** The below structure variable has elements corresponding to the above event ID's \n  
  Please refer \ref notify_val_s structure for more info*/
  notify_val_t notify_val;
  /*    union notify_val_s {
        uint8_t    player_status;
        uint32_t   track_num;
        uint32_t   track_pos;
        uint8_t    batter_status;
        uint8_t    system_status;
        player_att_val_list_t  att_val_list;
    } notify_val;*/
} avrcp_notify_t;

typedef struct rsi_bt_event_avrcp_notify_s {

  /** remote device address*/
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];

  /** Please refer \ref avrcp_notify_s structure for more info*/
  avrcp_notify_t notify_val;
} rsi_bt_event_avrcp_notify_t;
/** @addtogroup BT-CLASSIC7
* @{
*/
/**
 * @typedef    void (*rsi_bt_on_avrcp_connect_t)(uint16_t resp_status, rsi_bt_event_avrcp_connect_t *bt_event_avrcp_connect);
 * @brief      When Avrcp Connect event is raised from the module, this callback will be called. \n
 *             This event will be given by the module when avrcp connection happens from either side.
 * @param[out] resp_status, contains the response status (Success or Error code) \n 
 *             0              - SUCCESS \n 
 *             Non-Zero Value - ErrorCodes 
 * @note       Refer Bluetooth Generic Error Codes section upto 0x4FF8 from \ref error-codes .
 * @param[out] avrcp_conn, contains the avrcp connected information, please refer \ref rsi_bt_event_avrcp_connect_s \n
 *             structure for more info.
 * @return      void
 * @note       This callback has to be registered using rsi_bt_avrcp_register_callbacks API
 */
typedef void (*rsi_bt_on_avrcp_connect_t)(uint16_t resp_status, rsi_bt_event_avrcp_connect_t *bt_event_avrcp_connect);

/**
 * @typedef   void (*rsi_bt_on_avrcp_disconnect_t)(uint16_t resp_status,
 *                                            rsi_bt_event_avrcp_disconnect_t *bt_event_avrcp_disconnect);
 * @brief      When Avrcp Disconnect event is raised from the module, this callback will be called. \n
 *             This event will be given by the module when avrcp connection happens from either side.
 * @param[out] resp_status, contains the response status (Success or Error code) \n 
 *             0              - SUCCESS \n 
 *             Non-Zero Value - ErrorCodes 
 * @note       Refer Bluetooth Generic Error Codes section upto 0x4FF8 from \ref error-codes .
 * @param[out] avrcp_disconn, contains the avrcp disconnected information, please refer \ref rsi_bt_event_avrcp_disconnect_s \n
 *             structure for more info.
 * @return      void
 * @note       This callback has to be registered using rsi_bt_avrcp_register_callbacks API
 */
typedef void (*rsi_bt_on_avrcp_disconnect_t)(uint16_t resp_status,
                                             rsi_bt_event_avrcp_disconnect_t *bt_event_avrcp_disconnect);

/**
 * @typedef   void (*rsi_bt_on_avrcp_play_t)(uint16_t resp_status, rsi_bt_event_avrcp_play_t *bt_event_avrcp_play);
 * @brief      When Avrcp Play event is raised from the module, this callback will be called. \n
 *             This event will be given by the module when avrcp play button pressed/Released from Control Device.
 * @param[out] resp_status, contains the response status (Success or Error code) \n 
 *             0              - SUCCESS \n 
 *             Non-Zero Value - ErrorCodes 
 * @note       Refer Bluetooth Generic Error Codes section upto 0x4FF8 from \ref error-codes .
 * @param[out] avrcp_play, contains the avrcp play information, please refer \ref rsi_bt_event_avrcp_play_s \n
 *             structure for more info.
 * @return      void
 * @note       This callback has to be registered using rsi_bt_avrcp_register_callbacks API
 */
typedef void (*rsi_bt_on_avrcp_play_t)(uint16_t resp_status, rsi_bt_event_avrcp_play_t *bt_event_avrcp_play);

/**
 * @typedef    void (*rsi_bt_on_avrcp_pause_t)(uint16_t resp_status, rsi_bt_event_avrcp_pause_t *bt_event_avrcp_pause);
 * @brief      When Avrcp Pause event is raised from the module, this callback will be called. \n
 *             This event will be given by the module when avrcp pause button pressed/Released from Control Device.
 * @param[out] resp_status, contains the response status (Success or Error code) \n 
 *             0              - SUCCESS \n 
 *             Non-Zero Value - ErrorCodes 
 * @note       Refer Bluetooth Generic Error Codes section upto 0x4FF8 from \ref error-codes .
 * @param[out] avrcp_pause, contains the avrcp pause information, please refer \ref rsi_bt_event_avrcp_pause_s \n
 *             structure for more info.
 * @return      void
 * @note       This callback has to be registered using rsi_bt_avrcp_register_callbacks API
 */
typedef void (*rsi_bt_on_avrcp_pause_t)(uint16_t resp_status, rsi_bt_event_avrcp_pause_t *bt_event_avrcp_pause);

/**
 * @typedef    void (*rsi_bt_on_avrcp_stop_t)(uint16_t resp_status, rsi_bt_event_avrcp_stop_t *bt_event_avrcp_stop);
 * @brief      When Avrcp Stop event is raised from the module, this callback will be called. \n
 *             This event will be given by the module when avrcp stop button pressed/Released from Control Device.
 * @param[out] resp_status, contains the response status (Success or Error code) \n 
 *             0              - SUCCESS \n 
 *             non-zero value - errorcodes 
 * @note       refer bluetooth generic error codes section upto 0x4ff8 from \ref error-codes .
 * @param[out] avrcp_stop, contains the avrcp pause information, please refer \ref rsi_bt_event_avrcp_stop_s \n
 *             structure for more info.
 * @return      void
 * @note       This callback has to be registered using rsi_bt_avrcp_register_callbacks API
 */
typedef void (*rsi_bt_on_avrcp_stop_t)(uint16_t resp_status, rsi_bt_event_avrcp_stop_t *bt_event_avrcp_stop);

/**
 * @typedef    void (*rsi_bt_on_avrcp_next_t)(uint16_t resp_status, rsi_bt_event_avrcp_next_t *bt_event_avrcp_next);
 * @brief      When Avrcp next event is raised from the module, this callback will be called. \n
 *             This event will be given by the module when avrcp next button pressed/Released from Control Device.
 * @param[out] resp_status, contains the response status (Success or Error code) \n 
 *             0              - SUCCESS \n 
 *			   Non-Zero Value - ErrorCodes
 * @note       refer bluetooth generic error codes section upto 0x4ff8 from \ref error-codes .
 * @param[out] avrcp_next, contains the avrcp next information, please refer \ref rsi_bt_event_avrcp_next_s \n
 *             structure for more info.
 * @return      void
 * @note       This callback has to be registered using rsi_bt_avrcp_register_callbacks API
 */
typedef void (*rsi_bt_on_avrcp_next_t)(uint16_t resp_status, rsi_bt_event_avrcp_next_t *bt_event_avrcp_next);

/**
 * @typedef   void (*rsi_bt_on_avrcp_previous_t)(uint16_t resp_status,
 *                                          rsi_bt_event_avrcp_previous_t *bt_event_avrcp_previous);
 * @brief      When Avrcp Previous event is raised from the module, this callback will be called. \n
 *             This event will be given by the module when avrcp Previous button pressed/Released from Control Device.
 * @param[out] resp_status, contains the response status (Success or Error code) \n 
 *             0              - SUCCESS \n 
 *             non-zero value - errorcodes 
 * @note       refer bluetooth generic error codes section upto 0x4ff8 from \ref error-codes .
 * @param[out] avrcp_previous, contains the avrcp previous information, please refer \ref rsi_bt_event_avrcp_previous_s \n
 *             structure for more info.
 * @return      void
 * @note       This callback has to be registered using rsi_bt_avrcp_register_callbacks API
 */
typedef void (*rsi_bt_on_avrcp_previous_t)(uint16_t resp_status,
                                           rsi_bt_event_avrcp_previous_t *bt_event_avrcp_previous);

/**
 * @typedef    void (*rsi_bt_on_avrcp_vol_up_t)(uint16_t resp_status, rsi_bt_event_avrcp_vol_up_t *bt_event_avrcp_vol_up);
 * @brief      When Avrcp volume up event is raised from the module, this callback will be called. \n
 *             This event will be given by the module when avrcp volume up button pressed/Released from Control Device.
 * @param[out] resp_status, contains the response status (Success or Error code) \n 
 *             0              - SUCCESS \n 
 *             non-zero value - errorcodes 
 * @note       refer bluetooth generic error codes section upto 0x4ff8 from \ref error-codes .
 * @param[out] avrcp_vol_up, contains avrcp vol up info, please refer \ref rsi_bt_event_avrcp_vol_up_s \n
 *             structure for more info.
 * @return      void
 * @note       This callback has to be registered using rsi_bt_avrcp_register_callbacks API
 */
typedef void (*rsi_bt_on_avrcp_vol_up_t)(uint16_t resp_status, rsi_bt_event_avrcp_vol_up_t *bt_event_avrcp_vol_up);

/**
 * @typedef void (*rsi_bt_on_avrcp_vol_down_t)(uint16_t resp_status,
 *                                          rsi_bt_event_avrcp_vol_down_t *bt_event_avrcp_vol_down);
 * @brief      When Avrcp volume Down event is raised from the module, this callback will be called. \n
 *             This event will be given by the module when avrcp volume Down button pressed/Released from Control Device.
 * @param[out] resp_status, contains the response status (Success or Error code) \n 
 *             0              - SUCCESS \n 
 *             non-zero value - errorcodes 
 * @note       refer bluetooth generic error codes section upto 0x4ff8 from \ref error-codes .
 * @param[out] avrcp_vol_down, contains the avrcp vol down info, please refer \ref rsi_bt_event_avrcp_vol_down_s \n
 *             structure for more info. 
 * @return      void
 * @note       This callback has to be registered using rsi_bt_avrcp_register_callbacks API
 */
typedef void (*rsi_bt_on_avrcp_vol_down_t)(uint16_t resp_status,
                                           rsi_bt_event_avrcp_vol_down_t *bt_event_avrcp_vol_down);

/**
 * @typedef void (*rsi_bt_on_avrcp_get_elem_attr_resp_t)(uint16_t resp_status,
 *                                                    rsi_bt_event_avrcp_get_elem_attr_resp_t *avrcp_get_elem_attr_resp);
 * @brief      When Remote Devices Element Attributes is received this callback will be called.
 * @param[out] resp_status, contains the response status (Success or Error code) \n 
 *             0              - SUCCESS \n 
 *             non-zero value - errorcodes 
 * @note       refer bluetooth generic error codes section upto 0x4ff8 from \ref error-codes .
 * @param[out] avrcp_get_elem_attr_resp, contains the avrcp get elem attr resp info.
 * @return      void
 * @note       This callback has to be registered using rsi_bt_avrcp_register_callbacks API
 */
typedef void (*rsi_bt_on_avrcp_get_elem_attr_resp_t)(uint16_t resp_status,
                                                     rsi_bt_event_avrcp_get_elem_attr_resp_t *avrcp_get_elem_attr_resp);

/**
 * @typedef   void (*rsi_bt_on_avrcp_notify_event_t)(uint16_t resp_status,
 *                                              rsi_bt_event_avrcp_notify_t *bt_event_avrcp_notify);
 * @brief      Callback function is invoke when avrcp notification received from Control Device.
 * @param[out] resp_status, contains the response status (Success or Error code) \n 
 *             0              - SUCCESS \n 
 *             non-zero value - errorcodes 
 * @note       refer bluetooth generic error codes section upto 0x4ff8 from \ref error-codes .
 * @param[out] bt_event_avrcp_notify, contains the avrcp registered notifications information, \n
 *             please refer \ref rsi_bt_event_avrcp_notify_s structure for more info.
 * @return      void
 * @note       This callback has to be registered using rsi_bt_avrcp_register_callbacks API
 */
typedef void (*rsi_bt_on_avrcp_notify_event_t)(uint16_t resp_status,
                                               rsi_bt_event_avrcp_notify_t *bt_event_avrcp_notify);
/** @} */
/*==============================================*/

/**
 * @fn          rsi_bt_avrcp_register_callbacks 
 * @brief       This function registers the AVRCP profile callbacks
 * @param[in]   rsi_bt_on_avrcp_connect_t         bt_on_avrcp_connect_event              : avrcp connection status callback
 * @param[in]   rsi_bt_on_avrcp_disconnect_t      bt_on_avrcp_disconnect_event           : avrcp disconnection status callback
 * @param[in]   rsi_bt_on_avrcp_play_t            bt_on_avrcp_play_event                 : avrcp play status callback
 * @param[in]   rsi_bt_on_avrcp_pause_t           bt_on_avrcp_pause_event                : avrcp pause status callback
 * @param[in]   rsi_bt_on_avrcp_stop_t            bt_on_avrcp_stop_event                 : avrcp stop status callback
 * @param[in]   rsi_bt_on_avrcp_next_t            bt_on_avrcp_next_event                 : avrcp next status callback
 * @param[in]   rsi_bt_on_avrcp_previous_t        bt_on_avrcp_previous_event             : avrcp previous status callback
 * @param[in]   rsi_bt_on_avrcp_vol_up_t          bt_on_avrcp_vol_up_event               : avrcp vol up status callback
 * @param[in]   rsi_bt_on_avrcp_vol_down_t        bt_on_avrcp_vol_down_event             : avrcp vol down status callback
 * @return     
 *              Non zero - If fails
 *              0 - If success
 *
 *
 */
void rsi_bt_avrcp_register_callbacks(rsi_bt_on_avrcp_connect_t bt_on_avrcp_connect_event,
                                     rsi_bt_on_avrcp_disconnect_t bt_on_avrcp_disconnect_event,
                                     rsi_bt_on_avrcp_play_t bt_on_avrcp_play_event,
                                     rsi_bt_on_avrcp_pause_t bt_on_avrcp_pause_event,
                                     rsi_bt_on_avrcp_stop_t bt_on_avrcp_stop_event,
                                     rsi_bt_on_avrcp_next_t bt_on_avrcp_next_event,
                                     rsi_bt_on_avrcp_previous_t bt_on_avrcp_previous_event,
                                     rsi_bt_on_avrcp_vol_up_t bt_on_avrcp_vol_up_event,
                                     rsi_bt_on_avrcp_vol_down_t bt_on_avrcp_vol_down_event,
                                     rsi_bt_on_avrcp_get_elem_attr_resp_t bt_on_avrcp_get_elem_attr_resp_event,
                                     rsi_bt_on_avrcp_notify_event_t bt_on_avrcp_notify_event);

#define AVRCP_MAX_APP_SUPP_ATTS      5
#define AVRCP_MAX_APP_SUPP_ATT_VALS  5
#define AVRCP_MAX_APP_SUPP_CHAR_SETS 5
#define AVRCP_MAX_SONG_ATTS          10

typedef struct rsi_bt_event_avrcp_get_cur_att_val_s {

  /** remote device address*/
  uint8_t dev_addr[6];

  /** Number of Attributes*/
  uint8_t nbr_atts;

  /** Attributes List \n 
  #define AVRCP_MAX_APP_SUPP_ATTS  5*/
  uint8_t atts[AVRCP_MAX_APP_SUPP_ATTS];
} rsi_bt_event_avrcp_get_cur_att_val_t;

typedef struct rsi_bt_event_avrcp_set_att_val_s {

  /** remote device address*/
  uint8_t dev_addr[6];

  /** Number of Attributes*/
  uint8_t nbr_atts;

  /** Reserved for Future Use*/
  uint8_t reserved;

  /** Please refer \ref player_att_value_s structure for mote info \n 
  #define AVRCP_MAX_APP_SUPP_ATTS  5*/
  player_att_value_t att_list[AVRCP_MAX_APP_SUPP_ATTS];
} rsi_bt_event_avrcp_set_att_val_t;

typedef struct rsi_bt_event_avrcp_vals_text_req_s {

  /** remote device address*/
  uint8_t dev_addr[6];

  /** ID of the Attribute*/
  uint8_t att_id;

  /** Number of Values Present in the given att_id*/
  uint8_t nbr_vals;

  /** Values List \n 
  #define AVRCP_MAX_APP_SUPP_ATT_VALS  5*/
  uint8_t vals[AVRCP_MAX_APP_SUPP_ATT_VALS];
} rsi_bt_event_avrcp_vals_text_req_t;

typedef struct rsi_bt_event_avrcp_char_sets_s {

  /** remote device address*/
  uint8_t dev_addr[6];

  /** Number of Character Sets*/
  uint8_t nbr_sets;

  /** Reserved for Future Use*/
  uint8_t reserved;

  /** Character Sets List \n 
  #define AVRCP_MAX_APP_SUPP_CHAR_SETS 5*/
  uint16_t char_sets[AVRCP_MAX_APP_SUPP_CHAR_SETS];
} rsi_bt_event_avrcp_char_sets_t;

typedef struct rsi_bt_event_avrcp_ele_att_s {

  /** remote device address*/
  uint8_t dev_addr[6];

  /** Number of Attributes*/
  uint8_t nbr_atts;

  /** Reserved for Future Use*/
  uint8_t reserved;

  /** Element Attributes List of a Track \n 
  #define AVRCP_MAX_SONG_ATTS          10*/
  uint32_t ele_atts[AVRCP_MAX_SONG_ATTS];
} rsi_bt_event_avrcp_ele_att_t;

typedef struct rsi_bt_event_avrcp_set_abs_vol_s {

  /** remote device address*/
  uint8_t dev_addr[6];

  /** Absolute Volume*/
  uint8_t abs_vol;
  uint8_t remote_req_or_cmd_resp; //0 - remote request event, 1 - host cmd resp
} rsi_bt_event_avrcp_set_abs_vol_t;
typedef struct rsi_bt_event_avrcp_set_addr_player_s {

  /** remote device address*/
  uint8_t dev_addr[6];

  /** A Unique ID of a Media Player which is SET by Control Device*/
  uint16_t player_id;
} rsi_bt_event_avrcp_set_addr_player_t;

typedef struct rsi_bt_event_avrcp_get_folder_items_s {

  /** remote device address*/
  uint8_t dev_addr[6];

  /** Scope refers which List of folder items to be accessed at present \n
  0 - Media_Player_List \n
  1 - Virtual_File_System \n
  2 - Search \n 
  3 - NowPlaying */
  uint8_t scope;

  /** The offset within the listing of the item, which should be the first \n 
  returned item. The first element in the listing is at offset 0.*/
  uint16_t start_item;

  /** The offset within the listing of the item which should be the final returned item. \n 
  If this is set to a value beyond what is available, the TG shall return items from the \n 
  provided Start Item index to the index of the final item. \n 
  If the End Item index is smaller than the Start Item index, the TG shall return an error. \n 
  If CT requests too many items, TG can respond with a sub-set of the requested items. */
  uint16_t end_item;

  /** Request the Attributes present on the attr_cnt value \n 
  0x00 - All attributes are requested. \n 
  0x01-0xFE - The following Attribute List contains this number of attributes. \n 
  0xFF - No attributes are requested.*/
  uint8_t attr_cnt;
} rsi_bt_event_avrcp_get_folder_items_t;

typedef struct rsi_bt_event_avrcp_get_tot_num_items_s {

  /** This array holds the device address of length 6 bytes*/
  uint8_t dev_addr[6];

  /** Scope refers which List of folder items to be accessed at present \n
  0 - Media_Player_List \n
  1 - Virtual_File_System \n
  2 - Search \n 
  3 - NowPlaying */
  uint8_t scope;
} rsi_bt_event_avrcp_get_tot_num_items_t;
/** @addtogroup BT-CLASSIC7
* @{
*/
/**
 * @typedef void (*rsi_bt_on_avrcp_get_cap_event_t)(uint8_t *bd_addr, uint8_t cap_type);
 * @brief      Callback function is invoke when we receive get capabilities request from remote device.
 * @param[out] bd_addr, Remote bd address.
 * @param[out] cap_type, avrcp capabilities type.
 * @return      void
 * @note       This callback has to be registered using rsi_bt_avrcp_target_register_callbacks API
 */
typedef void (*rsi_bt_on_avrcp_get_cap_event_t)(uint8_t *bd_addr, uint8_t cap_type);

/**
 * @typedef   void (*rsi_bt_on_avrcp_get_app_supp_atts_event_t)(uint8_t *bd_addr);
 * @brief      Callback function is invoke when we receive get application support attribute list request from remote device.
 * @param[out] bd_addr, Remote bd address.
 * @return     void
 * @note       This callback has to be registered using rsi_bt_avrcp_target_register_callbacks API
 */
typedef void (*rsi_bt_on_avrcp_get_app_supp_atts_event_t)(uint8_t *bd_addr);

/**
 * @typedef    void (*rsi_bt_on_avrcp_get_app_suup_att_vals_event_t)(uint8_t *bd_addr, uint8_t att_id);
 * @brief      Callback function is invoke when we receive get application support attribute values request from remote device.
 * @param[out] bd_addr, Remote bd address.
 * @param[out] att_id, application attribute id.
 * @return      void
 * @note       This callback has to be registered using rsi_bt_avrcp_target_register_callbacks API
 */
typedef void (*rsi_bt_on_avrcp_get_app_suup_att_vals_event_t)(uint8_t *bd_addr, uint8_t att_id);

/**
 * @typedef    void (*rsi_bt_on_avrcp_get_app_cur_att_val_event_t)(rsi_bt_event_avrcp_get_cur_att_val_t *p_att_list);
 * @brief      Callback function is invoke when we receive get application current attributes value request from remote device.
 * @param[out] p_att_list, attribute list, please refer \ref rsi_bt_event_avrcp_get_cur_att_val_s structure \n
 *             for more info.
 * @return      void
 * @note       This callback has to be registered using rsi_bt_avrcp_target_register_callbacks API
 */
typedef void (*rsi_bt_on_avrcp_get_app_cur_att_val_event_t)(rsi_bt_event_avrcp_get_cur_att_val_t *p_att_list);

/**
 * @typedef    void (*rsi_bt_on_avrcp_set_app_cur_att_val_event_t)(rsi_bt_event_avrcp_set_att_val_t *p_att_list);
 * @brief      Callback function is invoke when we receive set application curent attributes value request from remote device.
 * @param[out] p_att_list, attribute list, please refer \ref rsi_bt_event_avrcp_set_att_val_s structure \n
 *             for more info.
 * @return      void
 * @note       This callback has to be registered using rsi_bt_avrcp_target_register_callbacks API
 */
typedef void (*rsi_bt_on_avrcp_set_app_cur_att_val_event_t)(rsi_bt_event_avrcp_set_att_val_t *p_att_list);

/**
 * @typedef void (*rsi_bt_on_avrcp_get_app_att_text_event_t)(rsi_bt_event_avrcp_get_cur_att_val_t *p_att_list);
 * @brief      Callback function is invoke when we receive get application attributes text request from remote device.
 * @param[out] p_att_list, attribute list, please refer \ref rsi_bt_event_avrcp_get_cur_att_val_s structure \n
 *             for more info.
 * @return      void
 * @note       This callback has to be registered using rsi_bt_avrcp_target_register_callbacks API
 */
typedef void (*rsi_bt_on_avrcp_get_app_att_text_event_t)(rsi_bt_event_avrcp_get_cur_att_val_t *p_att_list);

/**
 * @typedef   void (*rsi_bt_on_avrcp_get_app_att_vals_text_event_t)(rsi_bt_event_avrcp_vals_text_req_t *p_vals_list);
 * @brief      Callback function is invoke when we receive get application attribute values text request from remote device.
 * @param[out] p_vals_list, attribute values list, please refer \ref rsi_bt_event_avrcp_vals_text_req_s structure \n
 *             for more info.
 * @return      void
 * @note       This callback has to be registered using rsi_bt_avrcp_target_register_callbacks API
 */
typedef void (*rsi_bt_on_avrcp_get_app_att_vals_text_event_t)(rsi_bt_event_avrcp_vals_text_req_t *p_vals_list);

/**
 * @typedef    void (*rsi_bt_on_avrcp_supp_char_sets_event_t)(rsi_bt_event_avrcp_char_sets_t *p_char_sets);
 * @brief      Callback function is invoke when we receive inform character sets request from remote device.
 * @param[out] p_char_sets, support character sets list, please refer \ref rsi_bt_event_avrcp_char_sets_s structure \n
 *             for more info.
 * @return      void
 * @note       This callback has to be registered using rsi_bt_avrcp_target_register_callbacks API
 */
typedef void (*rsi_bt_on_avrcp_supp_char_sets_event_t)(rsi_bt_event_avrcp_char_sets_t *p_char_sets);

/**
 * @typedef void (*rsi_bt_on_avrcp_batt_status_event_t)(uint8_t *bd_addr, uint8_t batt_status);
 * @brief      Callback function is invoke when we receive battery status from remote device.
 * @param[out] bd_addr, Remote bd address.
 * @param[out] batt_status, battery status.
 * @return      void
 * @note       This callback has to be registered using rsi_bt_avrcp_target_register_callbacks API
 */
typedef void (*rsi_bt_on_avrcp_batt_status_event_t)(uint8_t *bd_addr, uint8_t batt_status);

/**
 * @typedef void (*rsi_bt_on_avrcp_get_song_atts_event_t)(rsi_bt_event_avrcp_ele_att_t *p_song_atts);
 * @brief      Callback function is invoke when we receive element/song attribute request from remote device.
 * @param[out] p_song_atts, requested song attribute list, please refer \ref rsi_bt_event_avrcp_ele_att_s structure \n
 *             for more info.
 * @return      void
 * @note       This callback has to be registered using rsi_bt_avrcp_target_register_callbacks API
 */
typedef void (*rsi_bt_on_avrcp_get_song_atts_event_t)(rsi_bt_event_avrcp_ele_att_t *p_song_atts);

/**
 * @typedef void (*rsi_bt_on_avrcp_get_play_status_event_t)(uint8_t *bd_addr);
 * @brief      Callback function is invoke when we receive player status from remote device.
 * @param[out] bd_addr, Remote bd address.
 * @return      void
 * @note       This callback has to be registered using rsi_bt_avrcp_target_register_callbacks API
 */
typedef void (*rsi_bt_on_avrcp_get_play_status_event_t)(uint8_t *bd_addr);

/**
 * @typedef    void (*rsi_bt_on_avrcp_reg_notify_event_t)(uint8_t *bd_addr, uint8_t event_id);
 * @brief      Callback function is invoke when we receive register notify event from remote device.
 * @param[out] bd_addr, Remote bd address.
 * @param[out] event_id, Event for which the Control Device requires notifications 
 * @return      void
 * @note       This callback has to be registered using rsi_bt_avrcp_target_register_callbacks API
 */
typedef void (*rsi_bt_on_avrcp_reg_notify_event_t)(uint8_t *bd_addr, uint8_t event_id);

/**
 * @typedef    void (*rsi_bt_on_avrcp_set_abs_vol_event_t)(rsi_bt_event_avrcp_set_abs_vol_t *p_abs_vol);
 * @brief      Callback function is invoke when we receive set absolute volume event from remote device.
 * @param[out] bd_addr, Remote bd address.
 * @param[out] p_abs_vol, avrcp absolute vol, please refer \ref rsi_bt_event_avrcp_set_abs_vol_s structure \n
 *             for more info.
 * @return      void
 * @note       This callback has to be registered using rsi_bt_avrcp_target_register_callbacks API
 */
typedef void (*rsi_bt_on_avrcp_set_abs_vol_event_t)(uint16_t resp_status, rsi_bt_event_avrcp_set_abs_vol_t *p_abs_vol);

/**
 * @typedef    void (*rsi_bt_on_avrcp_set_addr_player_event_t)(rsi_bt_event_avrcp_set_addr_player_t *p_set_addr_player);
 * @brief      Callback function is invoke when we receive set address player event from remote device.
 * @param[out] bd_addr, Remote bd address.
 * @param[out] p_set_addr_player,  inform the Target Device of which media player the Control Device wishes to control, \n
 *             please refer \ref rsi_bt_event_avrcp_set_addr_player_s structure for more info.
 * @return      void
 * @note       This callback has to be registered using rsi_bt_avrcp_target_register_callbacks API
 */
typedef void (*rsi_bt_on_avrcp_set_addr_player_event_t)(rsi_bt_event_avrcp_set_addr_player_t *p_set_addr_player);

/**
 * @callback   rsi_bt_on_avrcp_get_folder_items_event_t
 * @brief      Callback function is invoke when we receive get folder items event from remote device.
 * @param[out] bd_addr, Remote bd address.
 * @param[out] p_get_folder_items, to get the media player items list in a folder, please refer \ref rsi_bt_event_avrcp_get_folder_items_s \n
 *             structure for more info.
 * @return      void
 * @note       This callback has to be registered using rsi_bt_avrcp_target_register_callbacks API
 */
typedef void (*rsi_bt_on_avrcp_get_folder_items_event_t)(rsi_bt_event_avrcp_get_folder_items_t *p_get_folder_items);

/**
 * @callback   rsi_bt_on_avrcp_get_tot_num_items_event_t
 * @brief      Callback function is invoke when we receive get total number of items event from remote device.
 * @param[out] bd_addr, Remote bd address.
 * @param[out] p_get_tot_num_items, total number of items present in a folder, please refer \ref rsi_bt_event_avrcp_get_tot_num_items_s \n
 *             structure for more info.
 * @return      void
 * @note       This callback has to be registered using rsi_bt_avrcp_target_register_callbacks API
 */
typedef void (*rsi_bt_on_avrcp_get_tot_num_items_event_t)(rsi_bt_event_avrcp_get_tot_num_items_t *p_get_tot_num_items);
/** @} */
/*==============================================*/

/**
 * @fn          rsi_bt_avrcp_target_register_callbacks 
 * @brief       This function registers the avrcp target callbacks
 * @param[in]   rsi_bt_on_avrcp_get_cap_event_t                  bt_on_avrcp_gat_cap,
 * @param[in]   rsi_bt_on_avrcp_get_app_supp_atts_event_t        bt_on_avrcp_get_app_att_list,
 * @param[in]   rsi_bt_on_avrcp_get_app_suup_att_vals_event_t    bt_on_avrcp_get_app_att_vals_list,
 * @param[in]   rsi_bt_on_avrcp_get_app_cur_att_val_event_t      bt_on_avrcp_get_app_cur_att_val,
 * @param[in]   rsi_bt_on_avrcp_set_app_cur_att_val_event_t      bt_on_avrcp_set_app_cur_att_val,
 * @param[in]   rsi_bt_on_avrcp_get_app_att_text_event_t         bt_on_avrcp_get_app_att_text,
 * @param[in]   rsi_bt_on_avrcp_get_app_att_vals_text_event_t    bt_on_avrcp_get_app_att_vals_text,
 * @param[in]   rsi_bt_on_avrcp_supp_char_sets_event_t           bt_on_avrcp_supp_char_sets,
 * @param[in]   rsi_bt_on_avrcp_batt_status_event_t              bt_on_avrcp_batt_status,
 * @param[in]   rsi_bt_on_avrcp_get_song_atts_event_t            bt_on_avrcp_get_song_atts,
 * @param[in]   rsi_bt_on_avrcp_get_play_status_event_t          bt_on_avrcp_get_player_status,
 * @param[in]   rsi_bt_on_avrcp_reg_notify_event_t               bt_on_avrcp_reg_notifivation);
 * @param[in]   rsi_bt_on_avrcp_set_abs_vol_event_t              bt_on_avrcp_set_abs_vol);
 *
 * @return     
 *              Non zero - If fails
 *              0 - If success
 *
 *
 */
void rsi_bt_avrcp_target_register_callbacks(
  rsi_bt_on_avrcp_get_cap_event_t bt_on_avrcp_gat_cap,
  rsi_bt_on_avrcp_get_app_supp_atts_event_t bt_on_avrcp_get_app_att_list,
  rsi_bt_on_avrcp_get_app_suup_att_vals_event_t bt_on_avrcp_get_app_att_vals_list,
  rsi_bt_on_avrcp_get_app_cur_att_val_event_t bt_on_avrcp_get_app_cur_att_val,
  rsi_bt_on_avrcp_set_app_cur_att_val_event_t bt_on_avrcp_set_app_cur_att_val,
  rsi_bt_on_avrcp_get_app_att_text_event_t bt_on_avrcp_get_app_att_text,
  rsi_bt_on_avrcp_get_app_att_vals_text_event_t bt_on_avrcp_get_app_att_vals_text,
  rsi_bt_on_avrcp_supp_char_sets_event_t bt_on_avrcp_supp_char_sets,
  rsi_bt_on_avrcp_batt_status_event_t bt_on_avrcp_batt_status,
  rsi_bt_on_avrcp_get_song_atts_event_t bt_on_avrcp_get_song_atts,
  rsi_bt_on_avrcp_get_play_status_event_t bt_on_avrcp_get_player_status,
  rsi_bt_on_avrcp_reg_notify_event_t bt_on_avrcp_reg_notifivation,
  rsi_bt_on_avrcp_set_abs_vol_event_t bt_on_avrcp_set_abs_vol,
  rsi_bt_on_avrcp_set_addr_player_event_t bt_on_avrcp_set_addr_player,
  rsi_bt_on_avrcp_get_folder_items_event_t bt_on_avrcp_get_folder_items,
  rsi_bt_on_avrcp_get_tot_num_items_event_t bt_on_avrcp_get_tot_num_items);

/* HFP Profile */
typedef struct rsi_bt_event_hfp_connect_s {
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
} rsi_bt_event_hfp_connect_t;

typedef struct rsi_bt_event_hfp_disconnect_s {
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
} rsi_bt_event_hfp_disconnect_t;

typedef struct rsi_bt_event_hfp_ring_s {
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
} rsi_bt_event_hfp_ring_t;
typedef struct rsi_bt_event_hfp_callstatus_s {
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
  uint8_t call_status;
} rsi_bt_event_hfp_callstatus_t;

typedef struct rsi_bt_event_hfp_callsetup_s {
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
  uint8_t call_setup;
} rsi_bt_event_hfp_callsetup_t;

typedef struct rsi_bt_event_hfp_audioconn_s {
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
} rsi_bt_event_hfp_audioconn_t;

typedef struct rsi_bt_event_hfp_audiodisconn_s {
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
} rsi_bt_event_hfp_audiodisconn_t;

typedef struct rsi_bt_event_hfp_signalstrength_s {
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
  uint8_t signal_strength;
} rsi_bt_event_hfp_signalstrength_t;

typedef struct rsi_bt_event_hfp_batterylevel_s {
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
  uint8_t battery_status;
} rsi_bt_event_hfp_batterylevel_t;

typedef struct rsi_bt_event_hfp_dialcomp_s {
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
} rsi_bt_event_hfp_dialcomp_t;

typedef struct rsi_bt_event_hfp_anscomp_s {
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
} rsi_bt_event_hfp_anscomp_t;

typedef struct rsi_bt_event_hfp_hangupcomp_s {
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
} rsi_bt_event_hfp_hangupcomp_t;

typedef struct rsi_bt_event_hfp_senddtmfcomp_s {
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
} rsi_bt_event_hfp_senddtmfcomp_t;

typedef struct rsi_bt_event_hfp_phoneservice_s {
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
  uint8_t service;
} rsi_bt_event_hfp_phoneservice_t;

typedef struct rsi_bt_event_hfp_roamstatus_s {
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
  uint8_t status;
} rsi_bt_event_hfp_roamstatus_t;

typedef struct rsi_bt_event_hfp_calheldrelease_s {
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
} rsi_bt_event_hfp_calheldrelease_t;

typedef struct rsi_bt_event_hfp_callheld_s {
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
  uint8_t held_status;

} rsi_bt_event_hfp_callheld_t;

typedef struct rsi_bt_event_hfp_audio_codecselect_s {
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
  uint8_t codec_val;
} rsi_bt_event_hfp_audio_codecselect_t;

typedef struct rsi_bt_event_hfp_voice_data_s {
  uint16_t sco_handle;
  uint8_t length;
#ifdef MXRT_595s
  uint8_t voice_data[0];
#endif
} rsi_bt_event_hfp_voice_data_t;

typedef struct rsi_bt_event_hfp_calwait_s {
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
} rsi_bt_event_hfp_calwait_t;
typedef struct rsi_bt_event_hfp_servnotfound_s {
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
} rsi_bt_event_hfp_servnotfound_t;

typedef struct rsi_bt_event_hfp_voicerecogactivated_s {
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
} rsi_bt_event_hfp_voicerecogactivated_t;

typedef struct rsi_bt_event_hfp_voicerecogdeactivated_s {
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
} rsi_bt_event_hfp_voicerecogdeactivated_t;

typedef struct rsi_bt_event_hfp_dialnum_s {
  uint8_t dial_num[10];
} rsi_bt_event_hfp_dialnum_t;

typedef struct rsi_bt_event_hfp_spkgain_s {
  uint8_t spk_gain;
} rsi_bt_event_hfp_spkgain_t;

typedef struct rsi_bt_event_hfp_micgain_s {
  uint8_t mic_gain;
} rsi_bt_event_hfp_micgain_t;

typedef struct rsi_bt_event_hfp_phonenumber_s {
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
  uint8_t PhoneNbr[21];
  uint8_t PhoneType;
  uint8_t PhoneNbrType;
} rsi_bt_event_phonenum_t;

/******************************************************
 * *       BT HFP Response Callbacks Declarations
 * ******************************************************/

/**
 * @callback   rsi_bt_on_hfp_connect_t
 * @brief      Callback function to be called if profiles list response is received
 * @param[in]  resp_status, contains the response status (Success or Error code)
 * @param[in]  hfp_connect, contains the hfp connect information
 * @return      void
 * @section description
 * This callback function will be called if hfp connection happens.
 * This callback has to be registered using rsi_bt_spp_register_callbacks API
 */
typedef void (*rsi_bt_on_hfp_connect_t)(uint16_t resp_status, rsi_bt_event_hfp_connect_t *hfp_connect);
typedef void (*rsi_bt_on_hfp_disconnect_t)(uint16_t resp_status, rsi_bt_event_hfp_disconnect_t *hfp_disconn);
typedef void (*rsi_bt_on_hfp_ring_t)(uint16_t resp_status, rsi_bt_event_hfp_ring_t *hfp_ring);
typedef void (*rsi_bt_on_hfp_callcallerid_t)(uint16_t resp_status, rsi_bt_event_phonenum_t *phonenum);
typedef void (*rsi_bt_on_hfp_audioconnected_t)(uint16_t resp_status, rsi_bt_event_hfp_audioconn_t *audioconn);
typedef void (*rsi_bt_on_hfp_audiodisconnected_t)(uint16_t resp_status, rsi_bt_event_hfp_audiodisconn_t *audiodisconn);
typedef void (*rsi_bt_on_hfp_dialcomplete_t)(uint16_t resp_status, rsi_bt_event_hfp_dialcomp_t *dialcomp);
typedef void (*rsi_bt_on_hfp_answercomplete_t)(uint16_t resp_status, rsi_bt_event_hfp_anscomp_t *anscomp);
typedef void (*rsi_bt_on_hfp_hangupcomplete_t)(uint16_t resp_status, rsi_bt_event_hfp_hangupcomp_t *hangupcomp);
typedef void (*rsi_bt_on_hfp_senddtmfcomplete_t)(uint16_t resp_status, rsi_bt_event_hfp_senddtmfcomp_t *senddtmfcomp);
typedef void (*rsi_bt_on_hfp_callwait_t)(uint16_t resp_status, rsi_bt_event_hfp_calwait_t *callwait);
typedef void (*rsi_bt_on_hfp_callvoicerecogdeactivated_t)(
  uint16_t resp_status,
  rsi_bt_event_hfp_voicerecogdeactivated_t *voicerecog_deactivated);
typedef void (*rsi_bt_on_hfp_callvoicerecogactivated_t)(uint16_t resp_status,
                                                        rsi_bt_event_hfp_voicerecogactivated_t *voicerecog_activated);
typedef void (*rsi_bt_on_hfp_servicenotfound_t)(uint16_t resp_status, rsi_bt_event_hfp_servnotfound_t *servnotfound);
typedef void (*rsi_bt_app_on_hfp_callstatus_t)(uint16_t resp_status, rsi_bt_event_hfp_callstatus_t *callstatus);
typedef void (*rsi_bt_app_on_hfp_signalstrength_t)(uint16_t resp_status,
                                                   rsi_bt_event_hfp_signalstrength_t *signalstrength);
typedef void (*rsi_bt_app_on_hfp_batterylevel_t)(uint16_t resp_status, rsi_bt_event_hfp_batterylevel_t *batterylevel);
typedef void (*rsi_bt_app_on_hfp_phoneservice_t)(uint16_t resp_status, rsi_bt_event_hfp_phoneservice_t *phoneservice);
typedef void (*rsi_bt_app_on_hfp_roamingstatus_t)(uint16_t resp_status, rsi_bt_event_hfp_roamstatus_t *roamstatus);
typedef void (*rsi_bt_app_on_hfp_callsetup_t)(uint16_t resp_status, rsi_bt_event_hfp_callsetup_t *callsetup);
typedef void (*rsi_bt_app_on_hfp_callheld_t)(uint16_t resp_status, rsi_bt_event_hfp_callheld_t *callheld);
typedef void (*rsi_bt_app_on_hfp_voice_data_t)(uint16_t resp_status, rsi_bt_event_hfp_voice_data_t *voice_data);
typedef void (*rsi_bt_app_on_hfp_audio_codecselect_t)(uint16_t resp_status,
                                                      rsi_bt_event_hfp_audio_codecselect_t *codecvalue);

/******************************************************
 * * BT HFP Callbacks register function Declarations
 * ******************************************************/

/*==============================================*/
/**
 * @fn          rsi_bt_hfp_register_callbacks 
 * @brief       This function registers the HFP callbacks
 * @param[in]   rsi_bt_on_hfp_connect_t         bt_on_hfp_connect_event              : HFP connection status callback
 * @param[in]   rsi_bt_on_hfp_disconnect_t      bt_on_hfp_disconnect_event           : HFP disconnection status callback
 * @param[in]   rsi_bt_on_hfp_ring_t            bt_on_hfp_ring_event                 : HFP call ring status callback
 * @return     
 *              Non zero - If fails
 *              0 - If success
 *
 */

void rsi_bt_hfp_register_callbacks(rsi_bt_on_hfp_connect_t bt_on_hfp_connect_event,
                                   rsi_bt_on_hfp_disconnect_t bt_on_hfp_disconnect_event,
                                   rsi_bt_on_hfp_ring_t bt_on_hfp_ring_event,
                                   rsi_bt_on_hfp_callcallerid_t bt_on_hfp_callcallerid_event,
                                   rsi_bt_on_hfp_audioconnected_t bt_on_hfp_audioconnected_event,
                                   rsi_bt_on_hfp_audiodisconnected_t bt_on_hfp_audiodisconnected_event,
                                   rsi_bt_on_hfp_dialcomplete_t bt_on_hfp_dialcomplete_event,
                                   rsi_bt_on_hfp_answercomplete_t bt_on_hfp_answercomplete_event,
                                   rsi_bt_on_hfp_hangupcomplete_t bt_on_hfp_hangupcomplete_event,
                                   rsi_bt_on_hfp_senddtmfcomplete_t bt_on_hfp_senddtmfcomplete_event,
                                   rsi_bt_on_hfp_callwait_t bt_on_hfp_callwait_event,
                                   rsi_bt_on_hfp_callvoicerecogdeactivated_t bt_on_hfp_callvoicerecogdeactivated_event,
                                   rsi_bt_on_hfp_callvoicerecogactivated_t bt_on_hfp_callvoicerecogactivated_event,
                                   rsi_bt_on_hfp_servicenotfound_t bt_on_hfp_servicenotfound_event,
                                   rsi_bt_app_on_hfp_callstatus_t bt_on_hfp_callstatus_event,
                                   rsi_bt_app_on_hfp_signalstrength_t bt_on_hfp_signalstrength_event,
                                   rsi_bt_app_on_hfp_batterylevel_t bt_on_hfp_batterylevel_event,
                                   rsi_bt_app_on_hfp_phoneservice_t bt_on_hfp_phoneservice_event,
                                   rsi_bt_app_on_hfp_roamingstatus_t bt_on_hfp_roamingstatus_event,
                                   rsi_bt_app_on_hfp_callsetup_t bt_on_hfp_callsetup_event,
                                   rsi_bt_app_on_hfp_callheld_t bt_on_hfp_callheld_event,
                                   rsi_bt_app_on_hfp_voice_data_t bt_on_hfp_voice_data_event,
                                   rsi_bt_app_on_hfp_audio_codecselect_t bt_on_hfp_audio_codecselect_event);

/* PBAP Profile */
typedef struct rsi_bt_event_pbap_connect_s {
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
} rsi_bt_event_pbap_connect_t;

typedef struct rsi_bt_event_pbap_disconnect_s {
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
} rsi_bt_event_pbap_disconnect_t;

typedef struct rsi_bt_event_pbap_data_s {
  uint8_t PhoneNbr[13];
  uint8_t Name[30];
} rsi_bt_event_pbap_data_t;

/******************************************************
 * *       BT PBAP Response Callbacks Declarations
 * ******************************************************/

/**
 * @typedef void (*rsi_bt_on_pbap_connect_t)(uint16_t resp_status, rsi_bt_event_pbap_connect_t *pbap_connect);
 * @brief      Callback function to be called if pbap connect response is received
 * @param[in]  resp_status, contains the response status (Success or Error code)
 * @param[in]  pbap_connect, contains the pbap connect information
 * @return      void
 */
typedef void (*rsi_bt_on_pbap_connect_t)(uint16_t resp_status, rsi_bt_event_pbap_connect_t *pbap_connect);
/**
 * @typedef    void (*rsi_bt_on_pbap_disconnect_t)(uint16_t resp_status, rsi_bt_event_pbap_disconnect_t *pbap_disconn);
 * @brief      Callback function to be called if pbap disconnect response is received
 * @param[in]  resp_status, contains the response status (Success or Error code)
 * @param[in]  pbap_disconnect, contains the pbap connect information
 * @return      void
 */

typedef void (*rsi_bt_on_pbap_disconnect_t)(uint16_t resp_status, rsi_bt_event_pbap_disconnect_t *pbap_disconn);
/**
 * @typedef    void (*rsi_bt_on_pbap_data_t)(uint16_t resp_status, rsi_bt_event_pbap_data_t *pbap_data);
 * @brief      Callback function to be called if pbap data response is received
 * @param[in]  resp_status, contains the response status (Success or Error code)
 * @param[in]  pbap_data, contains the pbap data information
 * @return      void
 */

typedef void (*rsi_bt_on_pbap_data_t)(uint16_t resp_status, rsi_bt_event_pbap_data_t *pbap_data);

/******************************************************
 * * BT PBAP Callbacks register function Declarations
 * ******************************************************/

/*==============================================*/
/**
 * @fn          rsi_bt_pbap_register_callbacks 
 * @brief       This function registers the PBAP callbacks
 * @param[in]   rsi_bt_on_pbap_connect_t         bt_on_pbap_connect_event              : PBAP connection status callback
 * @param[in]   rsi_bt_on_pbap_disconnect_t      bt_on_pbap_disconnect_event           : PBAP disconnection status callback
 * @param[in]   rsi_bt_on_pbap_data_t            bt_on_pbap_data_event                 : PBAP data status callback
 * @return     
 *              Non zero - If fails
 *              0 - If success
 *
 *
 */

void rsi_bt_pbap_register_callbacks(rsi_bt_on_pbap_connect_t bt_on_pbap_connect_event,
                                    rsi_bt_on_pbap_disconnect_t bt_on_pbap_disconnect_event,
                                    rsi_bt_on_pbap_data_t bt_on_pbap_data_event);
/*==============================================*/
/*
 * @fn         rsi_bt_add_device_id
 * @brief      To add device identification for local device
 * @param[in]  Specification ID
 *             Vendor ID
 *             Product ID
 *             Version
 *             Primary record
 *             Vendor ID source
 * @return     int32_t
 *             0  =  success
 *             !0 = failure
 * @section description
 * This function is used to add the device id in the SDP.
  
-------------------------------------------------------------------------------------------------*/
int32_t rsi_bt_add_device_id(uint16_t spec_id,
                             uint16_t vendor_id,
                             uint16_t product_id,
                             uint16_t version,
                             int primary_rec,
                             uint16_t vendor_id_source);

/**
 * @fn         rsi_bt_set_sdp_attr_id
 */
uint32_t rsi_bt_set_sdp_attr_id(rsi_sdp_att_record_t *att_rec,
                                uint16_t attr_id,
                                uint16_t pattr_buf_idx,
                                uint16_t attr_buf_len);

/**
 * @fn         rsi_bt_add_sdp_attribute
 */
uint32_t rsi_bt_add_sdp_attribute(rsi_sdp_att_record_t *att_rec,
                                  uint16_t attr_id,
                                  uint8_t att_data_8,
                                  uint16_t att_data_16,
                                  uint8_t is_boolean,
                                  uint8_t param_len);

/**
 * @fn         rsi_bt_add_sdp_hid_language_attribute
 */
uint32_t rsi_bt_add_sdp_hid_language_attribute(rsi_sdp_att_record_t *att_record_data,
                                               uint16_t lang_id,
                                               uint16_t lang_attr_base);

/**
 * @fn         rsi_bt_add_sdp_hid_descriptor_list
 */
uint32_t rsi_bt_add_sdp_hid_descriptor_list(rsi_sdp_att_record_t *att_record_data, uint8_t *buff_ptr, uint8_t buff_len);

/**
 * @fn         rsi_bt_add_sdp_service_attribute
 */
uint32_t rsi_bt_add_sdp_service_attribute(rsi_sdp_att_record_t *att_rec,
                                          char *service_name,
                                          uint8_t name_len,
                                          uint16_t attr_id);

/**
 * @fn         rsi_bt_add_sdp_service_classid
 */
uint32_t rsi_bt_add_sdp_service_classid(rsi_sdp_att_record_t *att_rec, uint16_t serv_class_uuid);

/**
 * @fn         rsi_bt_add_sdp_service_handle
 */
uint32_t rsi_bt_add_sdp_service_handle(rsi_sdp_att_record_t *att_rec, uint32_t serv_hndl);

/**
 * @fn         rsi_bt_add_sdp_protocol_list
 */
uint32_t rsi_bt_add_sdp_protocol_list(rsi_sdp_att_record_t *att_rec,
                                      bt_sdp_proto_desc_list_elem_t *list,
                                      uint8_t list_cnt,
                                      uint16_t attr_id);

/**
 * @fn         rsi_bt_add_sdp_language_base_attributeid_list
 */
uint32_t rsi_bt_add_sdp_language_base_attributeid_list(rsi_sdp_att_record_t *att_rec,
                                                       bt_sdp_lang_attr_id_elem_t *list,
                                                       uint8_t list_cnt);

/**
 * @fn         rsi_bt_add_sdp_profile_descriptor_list
 */
uint32_t rsi_bt_add_sdp_profile_descriptor_list(rsi_sdp_att_record_t *att_rec,
                                                uint16_t profile_uuid,
                                                uint16_t profile_version);

/**
 * @fn         rsi_bt_add_sdp_service_record_handle
 */
uint32_t rsi_bt_add_sdp_service_record_handle(rsi_sdp_att_record_t *att_rec, uint32_t serv_hndl);

// A2DP Connect command
typedef struct rsi_bt_req_a2dp_connect_s {
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
} rsi_bt_req_a2dp_connect_t;

// A2DP disconnect command
typedef struct rsi_bt_req_a2dp_disconnect_s {
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
} rsi_bt_req_a2dp_disconnect_t;

// A2DP pcm/mp3 data structure
typedef struct rsi_bt_req_a2dp_pcm_mp3_data_s {
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
  uint16_t pcm_mp3_data_len : 12;
  uint16_t audio_type : 4;
  uint8_t pcm_mp3_data[512 * 7]; //[RSI_BT_MAX_PAYLOAD_SIZE];
} rsi_bt_req_a2dp_pcm_mp3_data_t;

// A2DP sbc data command
typedef struct rsi_bt_req_a2dp_sbc_aac_data_s {
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
  uint16_t sbc_aac_data_len : 12;
  uint16_t audio_type : 4;
  uint8_t reserved[13]; // RTP_HEADER_LEN
  uint8_t sbc_aac_data[RSI_BT_MAX_PAYLOAD_SIZE];
} rsi_bt_req_a2dp_sbc_aac_data_t;

// A2DP request start command
typedef struct rsi_bt_req_a2dp_start_s {
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
} rsi_bt_req_a2dp_start_t;

// A2DP request suspend command
typedef struct rsi_bt_req_a2dp_suspend_s {
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
} rsi_bt_req_a2dp_suspend_t;

// A2DP request close command
typedef struct rsi_bt_req_a2dp_close_s {
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
} rsi_bt_req_a2dp_close_t;

// A2DP request abort command
typedef struct rsi_bt_req_a2dp_abort_s {
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
} rsi_bt_req_a2dp_abort_t;

// AVRCP profile command
// AVRCP profile connect command
typedef struct rsi_bt_req_avrcp_conn_s {
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
} rsi_bt_req_avrcp_conn_t;

// AVRCP profile disconnect command
typedef struct rsi_bt_req_avrcp_disconnect_s {
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
} rsi_bt_req_avrcp_disconnect_t;

typedef struct rsi_bt_a2dp_sbc_codec_cap_s {

  /** Channel Mode for SBC \n
  #define SBC_CHANNEL_MODE_MONO         (1 << 3) \n
  #define SBC_CHANNEL_MODE_DUAL_CHANNEL (1 << 2) \n
  #define SBC_CHANNEL_MODE_STEREO       (1 << 1) \n
  #define SBC_CHANNEL_MODE_JOINT_STEREO (1 << 0)*/
  uint8_t ChannelMode : 4;

  /** Sampling Frequency for SBC \n
  #define SBC_SAMPLING_FREQ_16000 (1 << 3) \n
  #define SBC_SAMPLING_FREQ_32000 (1 << 2) \n
  #define SBC_SAMPLING_FREQ_44100 (1 << 1) \n
  #define SBC_SAMPLING_FREQ_48000 (1 << 0)*/
  uint8_t Freq : 4;

  /** Allocation Method for SBC \n
  #define SBC_ALLOCATION_SNR      (1 << 1) \n
  #define SBC_ALLOCATION_LOUDNESS (1 << 0)*/
  uint8_t AllocMethod : 2;

  /** Number of Subbands for SBC \n
  #define SBC_SUBBANDS_4 (1 << 1) \n
  #define SBC_SUBBANDS_8 (1 << 0)*/
  uint8_t SubBands : 2;

  /** Block Length for SBC \n
  #define SBC_BLOCK_LENGTH_4  (1 << 3) \n
  #define SBC_BLOCK_LENGTH_8  (1 << 2) \n
  #define SBC_BLOCK_LENGTH_12 (1 << 1) \n
  #define SBC_BLOCK_LENGTH_16 (1 << 0)*/
  uint8_t BlockLength : 4;

  /** The frame_length and bitrate will be calculated \n
  based on bitpool value and Sampling Frequency \n
  #define SBC_MIN_BITPOOL 2*/
  uint8_t MinBitPool;

  /** The frame_length and bitrate will be calculated \n
  based on bitpool value and Sampling Frequency \n
  #define SBC_MAX_BITPOOL 53 \n
  @note  In Coex Environment, we are supporting MAX_BITPOOL as 35*/
  uint8_t MaxBitPool;
} rsi_bt_a2dp_sbc_codec_cap_t;

typedef struct rsi_bt_req_a2dp_get_config_s {
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
} rsi_bt_req_a2dp_get_config_t;

typedef struct rsi_bt_resp_a2dp_get_config_s {

  /**Please refer \ref rsi_bt_a2dp_sbc_codec_cap_s structure for more info */
  rsi_bt_a2dp_sbc_codec_cap_t sbc_cap;
  uint32_t status;
} rsi_bt_resp_a2dp_get_config_t;

typedef struct rsi_bt_req_a2dp_set_config_s {
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
  rsi_bt_a2dp_sbc_codec_cap_t sbc_cap;
} rsi_bt_req_a2dp_set_config_t;

typedef struct rsi_bt_resp_a2dp_set_config_s {
  uint32_t err_status;
} rsi_bt_resp_a2dp_set_config_t;

// AVRCP profile play song command
typedef struct rsi_bt_req_avrcp_play_s {
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
} rsi_bt_req_avrcp_play_t;

// AVRCP profile pause song command
typedef struct rsi_bt_req_avrcp_pause_s {
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
} rsi_bt_req_avrcp_pause_t;

// AVRCP profile stop song command
typedef struct rsi_bt_req_avrcp_stop_s {
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
} rsi_bt_req_avrcp_stop_t;

// AVRCP profile next song command
typedef struct rsi_bt_req_avrcp_next_s {
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
} rsi_bt_req_avrcp_next_t;

// AVRCP profile previous song command
typedef struct rsi_bt_req_avrcp_previous_s {
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
} rsi_bt_req_avrcp_previous_t;

// AVRCP profile vol up command
typedef struct rsi_bt_req_avrcp_vol_up_s {
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
} rsi_bt_req_avrcp_vol_up_t;

// AVRCP profile vol down command
typedef struct rsi_bt_req_avrcp_vol_down_s {
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
} rsi_bt_req_avrcp_vol_down_t;

// AVRCP profile get capabilities command
typedef struct rsi_bt_req_avrcp_get_capabilities_s {
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
  uint8_t type;
} rsi_bt_req_avrcp_get_capabilities_t;

typedef struct rsi_bt_rsp_avrcp_get_capabilities_s {
  /** Capability Count*/
  uint32_t nbr_ids;

  /** List of Ids Supported to the given capability type*/
  uint32_t ids[RSI_MAX_ATT];
} rsi_bt_rsp_avrcp_get_capabilities_t;

// AVRCP profile get attributes list command
typedef struct rsi_bt_req_avrcp_get_att_list_s {
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
} rsi_bt_req_avrcp_get_att_list_t;

typedef struct rsi_bt_rsp_avrcp_get_atts_list_s {
  /** Number of Attributes Provided*/
  uint8_t nbr_ids;

  /** Array of player application Setting attribute ID \n
  #define BT_AVRCP_ILLEGAL_ATTRIBUTE     0x00 \n
  #define BT_AVRCP_EQUALIZER_ATTRIBUTE   0x01 \n
  #define BT_AVRCP_REPEAT_MODE_ATTRIBUTE 0x02 \n
  #define BT_AVRCP_SHUFFLE_ATTRIBUTE     0x03 \n
  #define BT_AVRCP_SCAN_ATTRIBUTE        0x04 */
  uint8_t att_ids[RSI_MAX_ATT];
} rsi_bt_rsp_avrcp_get_atts_list_t;

// AVRCP profile get attribute values list command
typedef struct rsi_bt_req_avrcp_get_att_vals_list_s {
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
  uint8_t att_id;
} rsi_bt_req_avrcp_get_att_vals_list_t;

typedef struct rsi_bt_rsp_avrcp_get_att_vals_list_s {
  /** Number of player application setting values*/
  uint8_t nbr_vals;

  /** Equalizer ON/OFF Status Attr Values \n 
  #define BT_AVRCP_EQUALIZER_OFF 0x01 \n
  #define BT_AVRCP_EQUALIZER_ON  0x02 \n

  Repeat Mode Status Attr Values \n
  #define BT_AVRCP_REPEAT_OFF    0x01 \n
  #define BT_AVRCP_REPEAT_SINGLE 0x02 \n 
  #define BT_AVRCP_REPEAT_ALL    0x03 \n 
  #define BT_AVRCP_REPEAT_GROUP  0x04 \n 

  Shuffle ON/OFF Status Attr Values \n 
  #define BT_AVRCP_SHUFFLE_OFF   0x01 \n 
  #define BT_AVRCP_SHUFFLE_ALL   0x02 \n 
  #define BT_AVRCP_SHUFFLE_GROUP 0x03 \n 

  Scan ON/OFF Status Attr Values \n 
  #define BT_AVRCP_SCAN_OFF 0x01 \n 
  #define BT_AVRCP_SCAN_ALL 0x02*/
  uint8_t att_vals[RSI_MAX_ATT];
} rsi_bt_rsp_avrcp_get_att_vals_list_t;

// AVRCP profile get current attribute value command
typedef struct rsi_bt_req_avrcp_get_cur_att_val_s {
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
  uint8_t nbr_atts;
  uint8_t att_list[RSI_MAX_ATT];
} rsi_bt_req_avrcp_get_cur_att_val_t;

typedef struct rsi_bt_rsp_avrcp_get_cur_att_val_s {
  /** Number of player application setting values*/
  uint8_t nbr_vals;

  /** Equalizer ON/OFF Status Attr Values  \n 
  #define BT_AVRCP_EQUALIZER_OFF 0x01 \n 
  #define BT_AVRCP_EQUALIZER_ON  0x02 \n 

  Repeat Mode Status Attr Values \n 
  #define BT_AVRCP_REPEAT_OFF    0x01 \n 
  #define BT_AVRCP_REPEAT_SINGLE 0x02 \n 
  #define BT_AVRCP_REPEAT_ALL    0x03 \n 
  #define BT_AVRCP_REPEAT_GROUP  0x04 \n 

  Shuffle ON/OFF Status Attr Values \n 
  #define BT_AVRCP_SHUFFLE_OFF   0x01 \n 
  #define BT_AVRCP_SHUFFLE_ALL   0x02 \n 
  #define BT_AVRCP_SHUFFLE_GROUP 0x03 \n 

  Scan ON/OFF Status Attr Values \n 
  #define BT_AVRCP_SCAN_OFF 0x01 \n 
  #define BT_AVRCP_SCAN_ALL 0x02*/
  uint8_t att_vals[RSI_MAX_ATT];
} rsi_bt_rsp_avrcp_get_cur_att_val_t;

// AVRCP profile Set current attribute value command

typedef struct rsi_bt_req_avrcp_set_cur_att_val_s {
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
  uint8_t nbr_atts;
  uint8_t reserved;
  att_val_t att_val_list[RSI_MAX_ATT];
} rsi_bt_req_avrcp_set_cur_att_val_t;

// AVRCP profile get song attribute value command
typedef struct rsi_bt_req_avrcp_get_ele_att_s {
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
  uint8_t nbr_atts;
  uint8_t reserved;
  uint32_t att_list[RSI_MAX_ATT];
} rsi_bt_req_avrcp_get_ele_att_t;

typedef struct normal_time_s {
  uint32_t org_time;
  uint32_t min;
  uint32_t sec;
} normal_time_t;

typedef struct att_text_s {
  /** Specifies the attribute/Value ID to be written*/
  uint32_t id;

  /** Specifies the character set ID to be displayed on CT*/
  uint16_t char_set_id;

  /** Length of the player application setting attribute/Value string*/
  uint16_t attr_text_len;

  /** Specifies the player application setting attribute/Value string in specified character set.*/
  uint8_t att_text[31];
} att_text_t;
#define MAX_TEXT_LIST 7
typedef struct player_att_text_s {
  /** Please refer \ref att_text_s structure for more info*/
  att_text_t list[MAX_TEXT_LIST];

  /** Number of Attributes/Values Provided*/
  uint8_t nbr_atts;
} player_att_text_t;

typedef struct attr_list_s {
  /** Specifies the attribute ID to be written*/
  uint32_t id;

  /** Specifies the character set ID to be displayed on CT*/
  uint16_t char_set_id;

  /** Length of the value of the attribute*/
  uint16_t attr_len;

  /** Attribute Name in specified character set*/
  uint8_t attr_val[499];
} attr_list_t;
#define MAX_ATT_LIST 7
typedef struct elem_attr_list_s {
  /** Number of attributes provided*/
  uint8_t num_attrs;

  /** Please refer \ref attr_list_s structure for more info*/
  attr_list_t attr_list[MAX_ATT_LIST];
} elem_attr_list_t;

typedef struct rsi_bt_rsp_avrcp_get_ele_att_s {
  player_att_text_t player_attr;
} rsi_bt_rsp_avrcp_get_ele_att_t;

typedef struct rsi_bt_rsp_avrcp_elem_attr_s {
  /** Please refer \ref elem_attr_list_s structure for more info*/
  elem_attr_list_t elem_attr_list;
} rsi_bt_rsp_avrcp_elem_attr_t;

// AVRCP profile get player status command
typedef struct rsi_bt_req_avrcp_get_player_status_s {
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
} rsi_bt_req_avrcp_get_player_status_t;

typedef struct rsi_bt_rsp_avrcp_get_player_status_s {
  /** The total length of the playing song in milliseconds*/
  uint32_t song_len;

  /** The current position of the playing in milliseconds elapsed */
  uint32_t song_pos;

  /** Current Status of playing \n
  STOPPED   0x00\n
  PLAYING   0x01\n
  PAUSED    0x02\n
  FWD_SEEK  0x03\n
  REV_SEEK  0x04\n
  ERROR     0xFF*/
  uint8_t play_status;
} rsi_bt_rsp_avrcp_get_player_status_t;

// AVRCP profile register notification event command
typedef struct rsi_bt_req_avrcp_reg_notification_s {
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
  uint8_t event_id;
} rsi_bt_req_avrcp_reg_notification_t;

// AVRCP profile get player status command
typedef struct rsi_bt_req_avrcp_remote_version_s {
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
} rsi_bt_req_avrcp_remote_version_t;

typedef struct rsi_bt_rsp_avrcp_remote_version_s {

  /** AVRCP Profile Version Info of Remote Device*/
  uint16_t version;

  /** Reserved For Future Use*/
  uint16_t reserved;
} rsi_bt_rsp_avrcp_remote_version_t;

// AVRCP profile get attribute value text command
typedef struct rsi_bt_req_avrcp_get_att_val_text_s {
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
  uint8_t att_id;
  uint8_t nbr_vals;
  uint8_t vals[RSI_MAX_ATT];
} rsi_bt_req_avrcp_get_att_val_text_t;

// AVRCP profile send the battery status command
typedef struct rsi_bt_req_avrcp_batt_status_s {
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
  uint8_t batt_status;
} rsi_bt_req_avrcp_batt_status_t;

// AVRCP profile send the support character sets command
#define MAX_SUPP_VALS 5
typedef struct char_sets_s {
  uint16_t supp_vals[MAX_SUPP_VALS];
  uint8_t nbr_sets;
} char_sets_t;

typedef struct rsi_bt_req_avrcp_char_sets_s {
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
  uint8_t Reserved[2];
  char_sets_t char_sets;
} rsi_bt_req_avrcp_char_sets_t;

typedef struct rsi_bt_avrcp_set_abs_vol_s {
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
  uint8_t rev;
  uint8_t abs_vol;
} rsi_bt_avrcp_set_abs_vol_t;

#define AVRCP_SUPP_CMP_CAP_ID     2
#define AVRCP_SUPP_NOTIFYS_CAP_ID 3
#define MAX_CAPS                  10
typedef struct rsi_bt_avrcp_cap_resp_s {
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
  uint8_t cap_type;
  uint8_t nbr_caps;
  uint32_t caps[MAX_CAPS];
} rsi_bt_avrcp_cap_resp_t;

typedef struct rsi_bt_avrcp_att_list_resp_s {
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
  uint8_t nbr_atts;
  uint8_t atts[MAX_CAPS];
} rsi_bt_avrcp_att_list_resp_t;

typedef struct rsi_bt_avrcp_att_vals_list_resp_s {
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
  uint8_t nbr_vals;
  uint8_t vals[MAX_CAPS];
} rsi_bt_avrcp_att_vals_list_resp_t;

typedef struct rsi_bt_avrcp_cur_att_vals_resp_s {
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
  uint8_t reserved[2];
  att_val_list_t att_list;
} rsi_bt_avrcp_cur_att_vals_resp_t;

typedef struct rsi_bt_avrcp_set_att_vals_resp_s {
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
  uint8_t status;
} rsi_bt_avrcp_set_att_vals_resp_t;

typedef struct rsi_bt_avrcp_elem_attr_resp_s {
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
  uint8_t reserved[2];
  elem_attr_list_t elem_attr_list;
} rsi_bt_avrcp_elem_attr_resp_t;

typedef struct rsi_bt_avrcp_play_status_resp_s {
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
  uint8_t play_status;
  uint8_t reserved;
  uint32_t song_len;
  uint32_t song_pos;
} rsi_bt_avrcp_play_status_resp_t;

typedef union reg_notify_val_u {
  uint8_t play_status;
  uint8_t curr_track_idx[8];
  uint8_t curr_playback_pos[4];
  uint8_t batter_status;
  uint8_t system_status;
  uint8_t player_app_setting[3];
  uint16_t playerid;
  uint16_t uidcounter;
  uint8_t abs_vol;
} reg_notify_val_t;

typedef struct rsi_bt_avrcp_reg_notify_interim_resp_s {
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
  uint8_t event_id;
  uint8_t reserved;
  reg_notify_val_t reg_notify_val;
} rsi_bt_avrcp_reg_notify_interim_resp_t;

typedef struct rsi_bt_avrcp_reg_notifiy_resp_s {
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
  uint8_t status;
} rsi_bt_avrcp_reg_notify_resp_t;

typedef struct rsi_bt_avrcp_att_text_resp_s {
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
  uint8_t reserved[2];
  player_att_text_t atts_text;
} rsi_bt_avrcp_att_text_resp_t;

/* typedef union notify_val_s {
  uint8_t   player_status;
  uint32_t  track_num;
  uint32_t  track_pos;
  uint8_t   battery_status;
  uint8_t   system_status;
  att_val_list_t  att_val_list;
} notify_val_t; */

typedef struct rsi_bt_avrcp_notify_s {
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
  uint8_t event_id;
  uint8_t reserved;
  notify_val_t notify_val;
} rsi_bt_avrcp_notify_t;

typedef struct rsi_bt_avrcp_cmd_reject_s {
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
  uint8_t pdu_id;
  uint8_t status;
} rsi_bt_avrcp_cmd_reject_t;

typedef struct rsi_bt_avrcp_set_abs_vol_resp_s {
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
  uint8_t abs_vol;
} rsi_bt_avrcp_set_abs_vol_resp_t;

typedef struct rsi_bt_avrcp_set_addr_player_resp_s {
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
  uint8_t status;
} rsi_bt_avrcp_set_addr_player_resp_t;

typedef struct folder_items_resp_s {
  /** Identified by an UID*/
  uint16_t uidcntr;

  /** Number of items present in that Folder*/
  uint16_t numofitems;

  /** Item_Type \n
  1 - Media Player Item \n
  2 - Folder Item \n
  3 - Media Element Item \n
  @note But this structure is declared for Media_Player_Item*/
  uint8_t itemtype;

  /** Lenth of the Items*/
  uint16_t itemlen;

  /** A unique identifier for this media player.*/
  uint16_t playerid;

  /** There are currently four base types of media player \n
  BIT(0) - AUDIO_PLAYER (Eg: Mp3 Player) \n
  BIT(1) - VIDEO_PLAYER (Eg: Mpeg4 Player) \n
  BIT(2) - BROADCASTING_AUDIO (Eg: FM Radio) \n 
  BIT(3) - BROADCASTING_VIDEO (Eg: DVB-H Tuner) */
  uint8_t majorplayertype;

  /** There are Two defined SubTypes \n
  BIT(0) - AUDIO_BOOK \n
  BIT(1) - PODCAST*/
  uint32_t playersubtype;

  /** Current Play Status \n
  0 - STOPPED \n
  1 - PLAYING \n
  2 - PAUSED \n 
  3 - FWD_SEEK \n 
  4 - REV_SEEK \n
  FF - ERROR*/
  uint8_t playstatus;

  /** Each Player on a TG announces its features to the CT in the PlayerFeatureBitmask*/
  uint8_t featurebitmask[16];

  /** Specifies the character set ID to be displayed on CT*/
  uint16_t charsetid;

  /** Length of Displayable Name. The name length shall be limited such that a response \n
  to a GetFolderItems containing one media player item fits within the maximum size of \n 
  AVRCP PDU which can be received by the CT. */
  uint16_t disnamelen;

  /** Displayable name of player */
  uint8_t disname[11];
} folder_items_resp_t;

typedef struct rsi_bt_avrcp_get_folder_items_resp_s {
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
  folder_items_resp_t fldr_items;
  uint8_t status;
} rsi_bt_avrcp_get_folder_items_resp_t;

typedef struct rsi_bt_avrcp_get_tot_num_items_resp_s {
  uint8_t dev_addr[RSI_DEV_ADDR_LEN];
  uint8_t status;
  uint16_t uidcntr;
  uint32_t numofitems;
} rsi_bt_avrcp_get_tot_num_items_resp_t;
#define RSI_BT_IAP_MAX_DATA_LEN 300
typedef struct rsi_bt_iap_payload_s {
  uint16_t len;
  uint8_t data[1000];
} rsi_bt_iap_payload_t;

typedef struct rsi_bt_iap_cmds_info_s {
  uint16_t len;
  uint16_t cmd[20];
} rsi_bt_iap_cmds_info_t;

typedef struct rsi_bt_iap_File_transfer_info_s {
  uint16_t len;
  uint8_t state;
  uint8_t FileTransferId;
  uint8_t FileTransferSessionId;
  uint8_t data[RSI_BT_IAP_MAX_DATA_LEN];
} rsi_bt_iap_File_transfer_info_t;

int32_t rsi_bt_a2dp_init(rsi_bt_a2dp_sbc_codec_cap_t *sbc_cap);
int32_t rsi_bt_a2dp_connect(uint8_t *remote_dev_addr);
int32_t rsi_bt_a2dp_disconnect(uint8_t *remote_dev_addr);

#if (!TA_BASED_ENCODER)
int32_t rsi_bt_a2dp_send_pcm_mp3_data(uint8_t *remote_dev_addr,
                                      uint8_t *pcm_mp3_data,
                                      uint16_t pcm_mp3_data_len,
                                      uint8_t audio_type,
                                      uint16_t *bytes_consumed);
#else
int32_t rsi_bt_a2dp_send_pcm_mp3_data(uint8_t *remote_dev_addr,
                                      uint8_t *pcm_mp3_data,
                                      uint16_t pcm_mp3_data_len,
                                      uint8_t audio_type);
#endif
int32_t rsi_bt_a2dp_send_sbc_aac_data(uint8_t *remote_dev_addr,
                                      uint8_t *sbc_aac_data,
                                      uint16_t sbc_aac_data_len,
                                      uint8_t audio_type);
int32_t rsi_bt_a2dp_start(uint8_t *remote_dev_addr);
int32_t rsi_bt_a2dp_suspend(uint8_t *remote_dev_addr);
int32_t rsi_bt_a2dp_close(uint8_t *remote_dev_addr);
int32_t rsi_bt_a2dp_abort(uint8_t *remote_dev_addr);
int32_t rsi_bt_a2dp_get_config(uint8_t *remote_dev_addr, rsi_bt_resp_a2dp_get_config_t *sbc_resp_cap);
int32_t rsi_bt_a2dp_set_config(uint8_t *remote_dev_addr, rsi_bt_a2dp_sbc_codec_cap_t *set_sbc_cap, int32_t *status);
int32_t rsi_bt_app_sbc_encode(void);
int32_t rsi_sbc_encode(void);
int32_t rsi_bt_set_local_device_role(uint8_t *remote_dev_addr, uint8_t set_role, uint8_t *resp);
int32_t rsi_switch_proto(uint8_t type, void (*callback)(uint16_t mode, uint8_t *bt_disable));

int32_t rsi_bt_avrcp_init(uint8_t *avrcp_feature);
int32_t rsi_bt_avrcp_conn(uint8_t *remote_dev_addr);
int32_t rsi_bt_avrcp_disconn(uint8_t *remote_dev_addr);
int32_t rsi_bt_avrcp_play(uint8_t *remote_dev_addr);
int32_t rsi_bt_avrcp_pause(uint8_t *remote_dev_addr);
int32_t rsi_bt_avrcp_stop(uint8_t *remote_dev_addr);
int32_t rsi_bt_avrcp_next(uint8_t *remote_dev_addr);
int32_t rsi_bt_avrcp_previous(uint8_t *remote_dev_addr);
int32_t rsi_bt_avrcp_vol_up(uint8_t *remote_dev_addr);
int32_t rsi_bt_avrcp_vol_down(uint8_t *remote_dev_addr);
int32_t rsi_bt_avrcp_get_capabilities(uint8_t *remote_dev_addr,
                                      uint8_t capability_type,
                                      rsi_bt_rsp_avrcp_get_capabilities_t *cap_list);
int32_t rsi_bt_avrcp_get_att_list(uint8_t *remote_dev_addr, rsi_bt_rsp_avrcp_get_atts_list_t *att_list);
int32_t rsi_bt_avrcp_get_att_vals_list(uint8_t *remote_dev_addr,
                                       uint8_t att_id,
                                       rsi_bt_rsp_avrcp_get_att_vals_list_t *att_vals_list);
int32_t rsi_bt_avrcp_get_cur_att_val(uint8_t *remote_dev_addr,
                                     uint8_t *att_list,
                                     uint8_t nbr_atts,
                                     rsi_bt_rsp_avrcp_get_cur_att_val_t *att_vals_list);
int32_t rsi_bt_avrcp_set_cur_att_val(uint8_t *remote_dev_addr, att_val_t *val_list, uint8_t nbr_atts);
int32_t rsi_bt_avrcp_get_element_att(uint8_t *remote_dev_addr,
                                     uint8_t *att_ids,
                                     uint8_t nbr_atts,
                                     rsi_bt_rsp_avrcp_elem_attr_t *att_vals);
int32_t rsi_bt_avrcp_get_play_status(uint8_t *remote_dev_addr, rsi_bt_rsp_avrcp_get_player_status_t *play_status);
int32_t rsi_bt_avrcp_reg_notification(uint8_t *remote_dev_addr, uint8_t event_id, uint8_t *p_resp_val);
int32_t rsi_bt_avrcp_get_remote_version(uint8_t *remote_dev_addr, rsi_bt_rsp_avrcp_remote_version_t *version);
int32_t rsi_bt_avrcp_get_att_text(uint8_t *remote_dev_addr,
                                  uint8_t nbr_atts,
                                  uint8_t *p_atts,
                                  player_att_text_t *p_att_text_resp);
int32_t rsi_bt_avrcp_get_att_val_text(uint8_t *remote_dev_addr,
                                      uint8_t att_id,
                                      uint8_t nbr_vals,
                                      uint8_t *p_vals,
                                      player_att_text_t *p_att_text_resp);
int32_t rsi_bt_avrcp_batt_status(uint8_t *remote_dev_addr, uint8_t batt_level);
int32_t rsi_bt_avrcp_supp_char_sets(uint8_t *remote_dev_addr, uint8_t nbr_sets, uint16_t *p_sets);
int32_t rsi_bt_avrcp_set_abs_vol(uint8_t *remote_dev_addr, uint8_t abs_vol, uint8_t *p_resp_abs_vol);
int32_t rsi_bt_avrcp_cap_resp(uint8_t *remote_dev_addr, uint8_t cap_type, uint8_t nbr_caps, uint32_t *p_caps);
int32_t rsi_bt_avrcp_att_list_resp(uint8_t *remote_dev_addr, uint8_t nbr_atts, uint8_t *p_atts);
int32_t rsi_bt_avrcp_att_val_list_resp(uint8_t *remote_dev_addr, uint8_t nbr_vals, uint8_t *p_vals);
int32_t rsi_bt_avrcp_cur_att_val_resp(uint8_t *remote_dev_addr, uint8_t nbr_atts, att_val_t *p_att_vals);
int32_t rsi_bt_avrcp_set_cur_att_val_resp(uint8_t *remote_dev_addr, uint8_t status);
int32_t rsi_bt_avrcp_ele_att_resp(uint8_t *remote_dev_addr, uint8_t num_attrs, attr_list_t *p_attr_list);
int32_t rsi_bt_avrcp_play_status_resp(uint8_t *remote_dev_addr,
                                      uint8_t play_status,
                                      uint32_t song_len,
                                      uint32_t song_pos);
int32_t rsi_bt_avrcp_reg_notify_resp(uint8_t *remote_dev_addr,
                                     uint8_t event_id,
                                     uint8_t event_data_len,
                                     uint8_t *event_data);
int32_t rsi_bt_avrcp_att_text_resp(uint8_t *remote_dev_addr, uint8_t nbr_atts, att_text_t *p_att_text);
int32_t rsi_bt_avrcp_att_vals_text_resp(uint8_t *remote_dev_addr, uint8_t nbr_vals, att_text_t *p_vals_text);
int32_t rsi_bt_avrcp_batt_status_resp(uint8_t *remote_dev_addr, uint8_t status);
int32_t rsi_bt_avrcp_char_set_status_resp(uint8_t *remote_dev_addr, uint8_t status);
int32_t rsi_bt_avrcp_notify(uint8_t *remote_dev_addr, uint8_t event_id, notify_val_t *p_notify_val);
int32_t rsi_bt_avrcp_cmd_reject(uint8_t *remote_dev_addr, uint8_t pdu_id, uint8_t status);
int32_t rsi_bt_avrcp_set_abs_vol_resp(uint8_t *remote_dev_addr, uint8_t abs_vol);
int32_t rsi_bt_avrcp_set_addr_player_resp(uint8_t *remote_dev_addr, uint8_t status);
int32_t rsi_bt_avrcp_get_folder_items_resp(uint8_t *remote_dev_addr,
                                           uint8_t status,
                                           folder_items_resp_t folder_items_resp);
int32_t rsi_bt_avrcp_get_tot_num_items_resp(uint8_t *remote_dev_addr,
                                            uint8_t status,
                                            uint16_t uidcntr,
                                            uint32_t numofitems);

int32_t rsi_bt_write_current_iac_lap(uint8_t no_of_iaps, uint8_t *iap_lap_list);
int32_t rsi_bt_set_afh_host_channel_classification(uint8_t enable, uint8_t *channel_map);
int32_t rsi_bt_get_afh_host_channel_classification(uint8_t *status);
int32_t rsi_bt_enable_device_under_testmode(void);
int32_t rsi_bt_gatt_connect(uint8_t *remote_dev_addr);
int32_t rsi_bt_per_rx(uint32_t *bt_perrx);
int32_t rsi_bt_per_tx(uint32_t *bt_pertx);
int32_t rsi_bt_change_pkt_type(uint8_t *remote_dev_addr, uint16_t pkt_type);
int32_t rsi_bt_ptt_req(uint8_t mode);

int32_t rsi_bt_per_stats(uint8_t cmd_type, struct rsi_bt_per_stats_s *per_stats);
int32_t rsi_bt_vendor_avdtp_stats_enable(uint16_t avdtp_stats_enable, uint32_t avdtp_stats_rate);
int32_t rsi_bt_vendor_ar_enable(uint16_t enable);
int32_t rsi_memory_stats_enable(uint8_t protocol, uint8_t memory_stats_enable, uint32_t memory_stats_interval_ms);
int32_t rsi_bt_vendor_dynamic_pwr(uint16_t enable,
                                  uint8_t *remote_dev,
                                  uint8_t power_index_br,
                                  uint8_t power_index_2m,
                                  uint8_t power_index_3m);

int32_t rsi_bt_iap_send_control_session_data(rsi_bt_iap_payload_t *payload);
int32_t rsi_bt_iap2_update_send_cmds_info(rsi_bt_iap_cmds_info_t *cmd_list);
int32_t rsi_bt_iap2_update_recv_cmds_info(rsi_bt_iap_cmds_info_t *cmd_list);
int32_t rsi_bt_iap2_Init_File_Transfer(void);
int32_t rsi_bt_iap2_deInit_File_Transfer(void);
int32_t rsi_bt_iap2_Send_File_Transfer_State(rsi_bt_iap_File_transfer_info_t *payload_info);
int32_t rsi_bt_iap2_Send_File_Transfer_data(rsi_bt_iap_File_transfer_info_t *payload_info);
int32_t rsi_bt_spp_disconnect(uint8_t *remote_dev_addr);
int32_t rsi_bt_spp_transfer(uint8_t *remote_dev_addr, uint8_t *data, uint16_t length);
int32_t rsi_bt_hfp_init(void);
int32_t rsi_bt_hfp_connect(uint8_t *remote_dev_addr);
int32_t rsi_bt_hfp_disconnect(uint8_t *remote_dev_addr);
int32_t rsi_bt_hfp_phoneoperator(uint8_t *operator);
int32_t rsi_bt_hfp_callaccept(void);
int32_t rsi_bt_hfp_callreject(void);
int32_t rsi_bt_hfp_dialnum(uint8_t *phone_no);
int32_t rsi_bt_hfp_dialmem(uint8_t *mem_ix);
int32_t rsi_bt_hfp_redial(void);
int32_t rsi_bt_hfp_voicerecognitionactive(void);
int32_t rsi_bt_hfp_voicerecognitiondeactive(void);
int32_t rsi_bt_hfp_spkgain(uint8_t spk_gain);
int32_t rsi_bt_hfp_micgain(uint8_t mic_gain);
int32_t rsi_bt_hfp_getcalls(void);
int32_t rsi_bt_hfp_audiotransfer(uint8_t b_to_hf_transfer);
int32_t rsi_bt_hfp_audio(void *hfp_audio_pkt);
int32_t rsi_bt_pbap_init(void);
int32_t rsi_bt_pbap_connect(uint8_t *remote_dev_addr);
int32_t rsi_bt_pbap_disconnect(uint8_t *remote_dev_addr);
int32_t rsi_bt_pbap_contacts(uint8_t *remote_dev_addr, uint8_t folder_id, uint8_t nbr_contacts, uint8_t start_ix);
#endif
