/*******************************************************************************
* @file  rsi_bt_config_DEMO_57.h
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
 * @file     rsi_bt_config_DEMO_57.h
 * @version  0.1  
 * @date     01 Feb 2020
 *
 *
 *
 *  @brief : This file contain definitions and declarations of BT APIs.
 *
 *  @section Description  This file contains definitions and declarations required to
 *  configure BT module.
 *
 *
 */

#ifndef SAPIS_EXAMPLES_RSI_DEMO_APPS_INC_RSI_BT_CONFIG_COEX_MAX_APP_H
#define SAPIS_EXAMPLES_RSI_DEMO_APPS_INC_RSI_BT_CONFIG_COEX_MAX_APP_H

#include <rsi_common_app.h>
//#if COEX_MAX_APP
#include <rsi_bt_common.h>
#include <rsi_data_types.h>
/***********************************************************************************************************************************************/
//! BT_A2DP_SOURCE APP CONFIG defines
/***********************************************************************************************************************************************/

//! Enable this for dual pairing
#define RSI_BT_DUAL_PAIR_TEST 0

#if (RSI_BT_DUAL_PAIR_TEST == 0)
#define RSI_BT_REMOTE_BD_ADDR (void *)"EB:06:EF:6C:A7:15" /* Sony-MDR */
#endif

#define RSI_APP_AVDTP_ROLE INITIATOR_ROLE //! ACCEPTOR_ROLE

#define A2DP_BT_ONLY_CONNECTION 0 /* 1: Only BT connection and No Streaming  | 0: BT connection + Streaming */

#if BT_EIR_FRIENDLY_NAME_TEST
#define RSI_BT_REMOTE_DEV1_NAME "Name1"
#define RSI_BT_REMOTE_DEV2_NAME "Name2"
#endif

#define RUN_TIME_BT_DISABLE 0
#if RUN_TIME_BT_DISABLE
#define TEST_CASE_1     1 //! Enable & Disable and Enable without any BT activity.
#define TEST_CASE_2     0 //! Enable =>Inquiry Done=>Disable=>Enable=>Inquiry Start
#define TEST_CASE_3     0 //! Enable =>A2DP Stream => Disable=>Enable=>A2dp Stream
#define ERROR_TEST_CASE 0 //! Check error handling when bt activity going on and suspend cmd passed.
#define TEST_CASE_4     0 //! BT_enable=>BT_disable=>Deep_Sleep=>BT_enable back.
#define TEST_CASE_5     0 //! INIT=>BT_enable=>Deep_Sleep=>BT_disable=>BT_enable back.
#define TEST_CASE_6     0 //! INIT=>Deep_Sleep=>BT_enable=>BT_disable=>BT_enable back.
#define TEST_CASE_7     0 //! EVENT based Handling for bt disable and enable does enquiry in loop
#if (TEST_CASE_1 || TEST_CASE_2 || TEST_CASE_3 || TEST_CASE_4 || TEST_CASE_5 || TEST_CASE_6)
#define ENABLE_BACK_AFTER_10SEC 1
#endif
#endif

#define RSI_BT_DYNAMIC_PWR_INDEX 0
#define GAIN_INDEX_BR            9
#define GAIN_INDEX_2M            9
#define GAIN_INDEX_3M            9
#define RSI_BT_AVDTP_STATS       0
#define RSI_BT_AUTO_RATE         0
#define RSI_BT_AUTO_RATE_ENABLE  0

#if RSI_BT_AVDTP_STATS
#define BT_BR_EDR_ADAPTIVE       1
#define RUN_TIME_PKT_TYPE_CHANGE 1
#endif

#define RSI_PACKET_CHANGE_INDICATION 0

#define RSI_BT_MEMORY_STATS 0

#define MEMORY_STATS_INTERVAL 5000
#define STATS_ENABLE          1
#define STATS_DISABLE         0
#define BT_STATS              2

#if (RSI_BT_DUAL_PAIR_TEST == 1)
#define RSI_BT_REMOTE_BD_ADDR_2         (void *)"00:1E:7C:25:E9:4D" /* PHILLIPS 1 */
#define RSI_BT_REMOTE_BD_ADDR           (void *)"00:17:03:01:E1:28" /* Portornics */
#define INQUIRY_ENABLE                  1
#define INQUIRY_CONNECTION_SIMULTANEOUS 0
#endif

#define RSI_BT_LOCAL_NAME (void *)"A2DP_AVRCP_SOURCE_G"
#define PIN_CODE          "0000"

#define RSI_AUDIO_DATA_TYPE PCM_AUDIO

#define AVRCP_PROFILE_ENABLE 1

#define TA_BASED_ENCODER \
  0 //! Enable only to test when RSI_AUDIO_DATA_SRC==SD_BIN_FILE & RSI_AUDIO_DATA_TYPE==PCM_AUDIO only.

#if !(TA_BASED_ENCODER)
#define PCM_INPUT_BUFFER_SIZE 15 * 512 //In the order of 512 (subands*block_length*channels*2) input for now
#define ENCODER_IN_RS9116     0
#else
#define PCM_INPUT_BUFFER_SIZE 14 * 512
#define ENCODER_IN_RS9116     1
#endif
#define MP3_INPUT_BUFFER_SIZE 10 * 512

#define BIN_FILE    1
#define ARRAY       2
#define SD_BIN_FILE 3

#define RSI_AUDIO_DATA_SRC SD_BIN_FILE

#define RSI_MASTER_ROLE 0
#define RSI_SLAVE_ROLE  1

#define BD_ADDR_ARRAY_LEN      18
#define RSI_LINKKEY_REPLY_SIZE 16

#define RPS_LINK_KEY_SAVE (1)
#if RPS_LINK_KEY_SAVE
#define LK_READ_BUF_SZ (512)
#define MAX_LINK_KEYS  10
typedef uint8_t dev_address_type[RSI_DEV_ADDR_LEN];
#endif

/***********************************************************************************************************************************************/
//! application events list
/***********************************************************************************************************************************************/

//! enum for global state
typedef enum {
  INQUIRY,
  CONNECTED,
  AUTHENTICATED,
  A2DP_CONNECTED,
  A2DP_STREAM_OPEN,
  A2DP_STREAM_START,
  AVRCP_CONNECTED,
} app_state_t;

//! application events list
enum rsi_app_event_e {
  RSI_APP_EVENT_CONNECTED = 1,
  RSI_APP_EVENT_PINCODE_REQ,
  RSI_APP_EVENT_LINKKEY_SAVE,
  RSI_APP_EVENT_AUTH_COMPLT,
  RSI_APP_EVENT_DISCONNECTED,
  RSI_APP_EVENT_LINKKEY_REQ,
  //! ssp related defines
  RSI_APP_EVENT_PASSKEY_DISPLAY = 10,
  RSI_APP_EVENT_PASSKEY_REQUEST,
  RSI_APP_EVENT_SSP_COMPLETE,
  RSI_APP_EVENT_CONFIRM_REQUEST,
  //! a2dp related defines
  RSI_APP_EVENT_A2DP_CONN,
  RSI_APP_EVENT_A2DP_DISCONN,
  RSI_APP_EVENT_A2DP_OPEN = 17,
  RSI_APP_EVENT_A2DP_START,
  RSI_APP_EVENT_A2DP_SUSPEND,
  RSI_APP_EVENT_A2DP_ABORT,
  RSI_APP_EVENT_A2DP_CLOSE,
  RSI_APP_EVENT_A2DP_SBC_DATA,
  RSI_APP_EVENT_A2DP_PCM_DATA,
  RSI_APP_EVENT_A2DP_CONFIGURE,
  RSI_APP_EVENT_WLAN_PCM_DATA,
  RSI_APP_EVENT_WLAN_SBC_DATA,
};

#define RSI_APP_EVENT_CONNECT              0
#define RSI_APP_EVENT_CONNECTION_INITIATED 1
#define RSI_APP_EVENT_CONNECTED            2
#define RSI_APP_EVENT_PINCODE_REQ          3
#define RSI_APP_EVENT_LINKKEY_SAVE         4
#define RSI_APP_EVENT_AUTH_COMPLT          5
#define RSI_APP_EVENT_DISCONNECTED         6
#define RSI_APP_EVENT_LINKKEY_REQ          7
#define RSI_APP_EVENT_UNBOND_STATUS        8
#define RSI_APP_EVENT_CONFIRM_REQUEST      9

//! a2dp related defines
#define RSI_APP_EVENT_A2DP_CONFIGURE 10
#define RSI_APP_EVENT_A2DP_OPEN      11
#define RSI_APP_EVENT_A2DP_CONN      12
#define RSI_APP_EVENT_A2DP_DISCONN   13

//! avrcp related defines
#define RSI_APP_EVENT_AVRCP_CONN          14
#define RSI_APP_EVENT_AVRCP_DISCONN       15
#define RSI_APP_EVENT_AVRCP_PLAY          16
#define RSI_APP_EVENT_AVRCP_PAUSE         17
#define RSI_APP_EVENT_AVRCP_GET_REM_CAP   18
#define RSI_APP_EVENT_AVRCP_GET_CAP       19
#define RSI_APP_EVENT_AVRCP_GET_ATTS      20
#define RSI_APP_EVENT_AVRCP_GET_ATT_VALS  21
#define RSI_APP_EVENT_AVRCP_CUR_ATT_VALS  22
#define RSI_APP_EVENT_AVRCP_SET_ATT_VALS  23
#define RSI_APP_EVENT_AVRCP_ATT_TEXT      24
#define RSI_APP_EVENT_AVRCP_ATT_VALS_TEXT 25
#define RSI_APP_EVENT_AVRCP_NEXT          26
#define RSI_APP_EVENT_AVRCP_PREVIOUS      27
//#define RSI_APP_EVENT_AVRCP_CHAR_SET                24
//#define RSI_APP_EVENT_AVRCP_BATT_STATUS             25
#define RSI_APP_EVENT_AVRCP_SONG_ATTS       28
#define RSI_APP_EVENT_AVRCP_PLAY_STATUS     29
#define RSI_APP_EVENT_AVRCP_REG_EVENT       30
#define RSI_APP_EVENT_A2DP_SUSPEND          31
#define RSI_APP_EVENT_A2DP_RECONFIG         32
#define RSI_APP_EVENT_A2DP_START            33
#define RSI_APP_EVENT_AVDTP_STATS           34
#define RSI_APP_EVENT_MEMORY_STATS_RECEIVED 35
#define RSI_APP_EVENT_PACKET_CHANGE         36
#define RSI_APP_EVENT_AVRCP_VOL_UP          37
#define RSI_APP_EVENT_AVRCP_VOL_DOWN        38
#define RSI_APP_EVENT_AVRCP_GET_REM_VER     39
#define RSI_APP_EVENT_AVRCP_NOTIFY          40
#define RSI_APP_EVENT_AR_STATS              41
#define RSI_APP_EVENT_A2DP_MORE_DATA_REQ    42
#define RSI_APP_EVENT_A2DP_SBC_ENCODE       43
#define RSI_APP_EVENT_BT_DISABLED           44
#define RSI_APP_EVENT_REMOTE_NAME_REQ       45
/***********************************************************************************************************************************************/
//! Error codes
/***********************************************************************************************************************************************/
#define RSI_APP_ERR_NONE                       0x0000
#define RSI_APP_ERR_SOCK_DISCONN               0x4062
#define RSI_APP_ERR_SOCK_TIMEOUT               0x4063
#define RSI_APP_ERR_SDP_SRV_NOT_FOUND          0x4102
#define RSI_APP_ERR_PAGE_TIMEOUT               0x4E04
#define RSI_APP_ERR_PIN_MISSING                0x4E06
#define RSI_APP_ERR_CONN_TIMEOUT               0x4E08
#define RSI_APP_ERR_ACL_CONN_ALREADY_EXISTS    0x4E0B
#define RSI_APP_ERR_LIMITED_RESOURCE           0x4E0D
#define RSI_APP_ERR_REMOTE_TERMINATED_CONN     0x4E13
#define RSI_APP_ERR_LOCAL_TERMINATED_CONN      0x4E16
#define RSI_APP_ERR_REPEATED_ATTEMPTS          0x4E17
#define RSI_APP_ERR_LMP_RESP_TIMEOUT           0x4E22
#define RSI_APP_ERR_HW_BUFFER_OVERFLOW         0x4057
#define RSI_APP_ERR_HW_BUFFER_OVERFLOW_TIMEOUT 0x405D
#define RSI_APP_ERR_A2DP_CONN_ALRDY_EXISTS     0x4511
#define RSI_APP_ERR_A2DP_NOT_STREAMING         0x4512
#define RSI_APP_ERR_A2DP_INVALID_SOCKET        0x4049
#define RSI_APP_ERR_A2DP_SBC_SAME_CODEC_PARAMS 0x4513
#define RSI_APP_ERR_A2DP_RECONFIG_CMD_TIMEOUT  0x4514
#define RSI_APP_ERR_BT_SDP_RECORDS_NOT_FOUND   0x4102

#define RSI_APP_ERR_AUDIO_EOF 0x00A0

#define RSI_LINKKEY_REPLY_SIZE 16
/***********************************************************************************************************************************************/
//! avrcp related defines
/***********************************************************************************************************************************************/
#define SONG_NAME           0x01
#define ARTIST_NAME         0x02
#define ALBUM_NAME          0x03
#define SONG_NUMBER         0x04
#define NUM_OF_SONGS        0x05
#define SONG_GENRE          0x06
#define PLAY_TIME           0x07
#define ARTIST_NAME_1       "BT_AUDIO"
#define ALBUM_NAME_LOCAL    "Local"
#define GENRE_NAME_TELUGU   "Telugu"
#define PCM_DATA_LEN_PER_MS (44100 * 2 * 2)

//! Inquiry related defines
#ifndef INQUIRY_ENABLE
#define INQUIRY_ENABLE                  1 //! To scan for device and connect
#define INQUIRY_CONNECTION_SIMULTANEOUS 0 //! To check simultaneous inquiry + connectivity
#endif
#if INQUIRY_ENABLE
#define MAX_NO_OF_RESPONSES 10
#define INQUIRY_DURATION    10000
#define INQUIRY_TYPE        2
#define MAX_NAME_LENGTH     10
#endif
#include <rsi_bt_common_config.h>
#ifdef FW_LOGGING_ENABLE
/*=======================================================================*/
//! Firmware Logging Parameters
/*=======================================================================*/
//! Enable or Disable firmware logging (Enable = 1; Disable = 0)
#define FW_LOG_ENABLE 1
//! Set TSF Granularity for firmware logging in micro seconds
#define FW_TSF_GRANULARITY_US 10
//! Log level for COMMON component in firmware
#define COMMON_LOG_LEVEL FW_LOG_ERROR
//! Log level for CM_PM component in firmware
#define CM_PM_LOG_LEVEL FW_LOG_ERROR
//! Log level for WLAN_LMAC component in firmware
#define WLAN_LMAC_LOG_LEVEL FW_LOG_ERROR
//! Log level for WLAN_UMAC component in firmware
#define WLAN_UMAC_LOG_LEVEL FW_LOG_ERROR
//! Log level for WLAN NETWORK STACK component in firmware
#define WLAN_NETSTACK_LOG_LEVEL FW_LOG_ERROR
//! Log level for BT BLE CONTROL component in firmware
#define BT_BLE_CTRL_LOG_LEVEL FW_LOG_INFO
//! Log level for BT BLE STACK component in firmware
#define BT_BLE_STACK_LOG_LEVEL FW_LOG_INFO
//! Min Value = 2048 bytes; Max Value = 4096 bytes; Value should be in multiples of 512 bytes
#define FW_LOG_BUFFER_SIZE 2048
//! Set queue size for firmware log messages
#define FW_LOG_QUEUE_SIZE 2
#endif
//#endif
#endif
